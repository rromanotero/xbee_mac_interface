/**
* Copyright 2015 Rafael Roman Otero. 
*
* This file is part of Xbee MAC Interface.
*
* Xbee MAC Interface is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
 * @file	xbee.c
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief The Xbee S1 Library
 *
 * Supports API mode only, and it is not exhaustive (it lacks networking and
 * sleeping functionality)
 *
 * It is assumed:
 * a) Xbee's baudrate is the one specified in xbee_init,
 * b) Xbee is pre-configured in API mode.
 * c) The rest of the parameters are the default ones 
 * (i.e. before plugging restore xbee to default then set baudrate
 *       and API mode )
 */

#include <stdint-gcc.h>
#include <stdbool.h>
#include <stdio.h>
#include "xbee_uart.h"
#include "xbee_cpu.h"
#include "xbee.h"
#include "mac_config.h"


#define START_DELIMITER					0x7E	///< Start Delimiter value
#define API_ID_TX						0x01	///< API ID value for TX request (msg send) frame
#define API_ID_AT_COMMAND				0x08	///< API ID value for AT Command request frame
#define API_ID_AT_COMMAND_RESPONSE		0x88	///< API ID value for AT Command response frames
#define API_ID_MESSAGE_RESPONSE			0x89	///< API ID value for message response (ack) frames
#define API_ID_MESSAGE_RECEIVED_16bit	0x81	///< API ID value for 16-bit RX request (msg received) frame
#define API_ID_MESSAGE_RECEIVED_64bit	0x80	///< API ID value for 64-bit RX request (msg received) frame
#define API_ID_MODEM_STATUS				0x8A	///< API ID value for modem status request frame

typedef struct{ ///< TX Request API Frame
	uint8_t start_delimiter;
	uint16_t length;
	uint8_t command_id;
	uint8_t frame_id;
#ifdef XBEE_64_ADDR_MODE_ENABLED
	uint8_t dest_address[8];
#else
	uint8_t dest_address[2];
#endif
	uint8_t options;
	uint8_t* rf_data;
	uint8_t rf_data_length;
	uint8_t checksum;
}ApiFrameMsg; 

typedef struct{ ///< AT Command Request API Frame
	uint8_t start_delimiter;
	uint16_t length;
	uint8_t command_id;
	uint8_t frame_id;
	const uint8_t* at_command;
	const uint8_t* at_param;
	uint8_t at_param_length;
	uint8_t checksum;
}ApiFrameATCommand;	

typedef struct{ ///< AT Command Response API Frame
	uint8_t status;
	uint8_t value_requested[8];
	uint8_t value_requested_length;
}ApiFrameATCommandResponse; 


static bool read_at_command_response( XbeeATCommandResponse*, uint16_t );
static bool read_msg( Message*, uint16_t );
static bool read_msg_response( XbeeStatus*, uint8_t*, uint16_t );
static void send_at_command_frame( ApiFrameATCommand* );
static void create_at_command_frame( ApiFrameATCommand* );
static void send_msg_frame( ApiFrameMsg* );
static void create_msg_frame( ApiFrameMsg* );
static bool is_xbee_baudrate_correct( uint32_t );
static uint8_t baudrate_to_num( uint32_t );

//Data received (from Xbee) event 
static void data_received_callback(void);

//upper layer callbacks
static void (*app_msg_reponse_callback)(XbeeStatus, uint8_t);
static void (*app_msg_received_callback)(Message*);
static void (*app_at_cmd_response_callback)(XbeeATCommandResponse*);


/**
*	Registers the upper-layer message received callback.
*
*	Registers the upper-layer message received callback, which is called when
*	a message is received in the Xbee.
*
*	@param app_callback	the callback function
*/
void xbee_register_msg_received_callback( void (*app_callback)(Message*) ){
	app_msg_received_callback = app_callback; 
}

/**
*	Registers the upper-layer message responded callback
*
*	Registers the upper-layer message responded callback, which is called when
*	a message is responded by the Xbee. A message response can be an ACK
*	received, ACK not received (timeout), etc.
*
*	@param response_callback	the callback function
*/
void xbee_register_msg_responded_callback( void(*response_callback)(XbeeStatus, uint8_t) ){
	app_msg_reponse_callback = response_callback; 
}

/**
*	Registers the AT command responded callback
*
*	Registers the upper-layer AT command responded callback, which is called when
*	an AT command is responded by the Xbee. A message response depends on the
*	AT command issued previously.
*
*	@param response_callback	the callback function
*/
void xbee_register_at_command_responded_callback( void(*response_callback)(XbeeATCommandResponse*) ){
	app_at_cmd_response_callback = response_callback;
}

/**
*	Initializes the Xbee
*
*	Configures and initializes the Xbee, if connected as per xbee_uart.
*	It assumes The Xbee is pre-configured in API mode. This routine will halt if
*	Xbee is not present.
*
*	@param baudrate	baudrate desired
*
*	@return true if the Xbee's baudrate matches the specified one, false otherwise
*/
uint32_t xbee_init( uint32_t baudrate ){
	
	//sets data received callback
	xbee_uart_register_callback( data_received_callback ); //from uart to xbee (this is how xbee_uart notifies xbee of incoming data)
	
	//init uart
	xbee_uart_config_init(baudrate); //make sure xbee's baudrate matches this same baudrate
	
	
	//Check Xbee baud rate is correct
	if( !is_xbee_baudrate_correct(RADIO_SPEED_RATE) )
		return false;

	//enable UART interrupts
	xbee_uart_enable_interrupt();
	
	return true;
}


/**
*	Send message
*
*	Constructs and transmits a TX Request (msg delivery) command.
*
*	@param msg pointer to the msg to be sent
*	@param msg_id an id to be attached to the msg
*/
void xbee_send_msg(Message *msg, uint8_t msg_id ){
	ApiFrameMsg api_frame;

	//API ID
	api_frame.command_id = API_ID_TX;	//Tx request
	
	//Frame ID
	if( msg->address == MSG_BROADCAST_ADDRESS ){
		//disable response frame
		api_frame.frame_id = 0;
	}
	else{
		//enable response frame
		api_frame.frame_id = msg_id;
	}
	
	//Destination Address
	api_frame.dest_address[0] = (uint8_t)((msg->address)>>8);//MSB address ... careful here with endianness
	api_frame.dest_address[1] = (uint8_t)(msg->address); //LSB address
	
	//Options
	if( msg->address == MSG_BROADCAST_ADDRESS )
		api_frame.options = 0x01; //Disable ACK
	else
		api_frame.options = 0x00;
	
	//RF Data (up to 100 bytes)
	api_frame.rf_data = msg->data;
	
	//Length of RF data
	api_frame.rf_data_length = msg->data_length;
	
	//Length of API frame
	api_frame.length = msg->data_length + 5; //rf data length + options + destination (2) + frameid + cmd id
	
#ifdef XBEE_64_ADDR_MODE_ENABLED
	api_frame.length += 6; //six extra bytes for holding 64bit address
#endif
	
	//creates (fills the remaining of) a frame and sends
	create_msg_frame( &api_frame );
	send_msg_frame( &api_frame );
}


/**
*	Send AT Command
*
*	Constructs and transmits an AT Command Request command
*
*	@param command pointer to a null-terminated AT command
*	@param params pointer to a null-terminated list (string) of params
*	@param params_length number of parameters being passed
*/
void xbee_send_at_command( const uint8_t* command, const uint8_t* params, uint8_t params_length ){
	ApiFrameATCommand api_frame;

	//sets fields
	api_frame.command_id = API_ID_AT_COMMAND;	//At command request
	api_frame.frame_id = 0x4D;			//random frame id... no need to correlate 
	api_frame.length = 4 + params_length;// cmd_id+  frame_id + command (2) + params
	api_frame.at_command = command;
	api_frame.at_param = params;
	api_frame.at_param_length = params_length;
	
	//creates (fills) and sends
	create_at_command_frame( &api_frame );
	send_at_command_frame( &api_frame );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////		     					L O C A L     R O U T I N E S								//////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
*	Xbee Data Received Callback.
*
*	Lower-layer-to-Xbee data received event. It is executed every time 
*	data has being received over the UART. This is where incoming transmissions are
*	processed and delivered to upper layers accordingly. (This function is called 
*	from within UART1's handler)
*
*/
static void data_received_callback(void){
	
	//ignore until start delimiter found
	while( xbee_uart_getc() != START_DELIMITER );
	
	//length 
	uint16_t length = (uint16_t)(xbee_uart_getc())<<8;
	length += xbee_uart_getc();
	
	//Read API (cmd) Identifier
	uint8_t api_id = xbee_uart_getc();

	//report event (to upper layers) accordingly
	XbeeATCommandResponse response;
	XbeeStatus msg_status;
	uint8_t msg_id;
	Message msg;
	
	switch(api_id){
		
		case API_ID_AT_COMMAND_RESPONSE: 
			// --- AT Commands response received --
			
			if( read_at_command_response(&response, length) ){
				//notify app
				(*app_at_cmd_response_callback)(&response);
			}
			else{
				//wrong checksum...not notify... fix this 
			}
			break;
			
		case API_ID_MESSAGE_RESPONSE: 
			// --- Message response received ---
			
			if( read_msg_response( &msg_status, &msg_id, length ) ){
				//notify app
				(*app_msg_reponse_callback)(msg_status, msg_id);
			}
			else{
				//wrong checksum... not notify... fix this later
			}
			break;
			
		case API_ID_MESSAGE_RECEIVED_16bit: 
			// --- Message received (16-bit address version) ---
			
			if( read_msg(&msg, length) ){
				//notify app
				(*app_msg_received_callback)(&msg);
			}
			else{
				//wrong checksum... notify app?.... fix this later
			
			}
			break;
			
		case API_ID_MESSAGE_RECEIVED_64bit:
			// ---- ignore msg's using 64bit address ---- FOR NOW
			for(uint16_t i=0; i<length; i++){
				xbee_uart_getc();	
			}
			break;
			
		case API_ID_MODEM_STATUS:
			// ---- ignore modem status ---- FOR NOW
			//This is not used unless certain event occur in the Xbee
			// (e.g. Watchdog timer reset, Coordinator started, etc)
			// With the functionality included so far, no modem
			// status should be received.

			xbee_uart_getc();
			xbee_uart_getc();
			break;
		
		default:
			//something went wrong
			break;
	}

}


/**
*	Read AT Command response
*
*	Reads an AT command response API frame from the UART.  
*
*	@param response pointer to the AT command response to be populated
*	@param length length of the API frame data 
*
*	@return true if checksum was OK
*/
static bool read_at_command_response( XbeeATCommandResponse *response, uint16_t length ){

	response->value_requested_length = length - 5;
	
	//ignore frame id
	xbee_uart_getc();
	
	//ignore AT command
	xbee_uart_getc();
	xbee_uart_getc();
	
	//read status
	response->status = xbee_uart_getc();
	
	//reads value requested
	for(uint32_t i=0; i<response->value_requested_length;i++){
		response->value_requested[i] = xbee_uart_getc();
	}
	
	//ignore checksum..... fix this!
	xbee_uart_getc();
	
	return true;
}

/**
*	Read message
*
*	Reads a message receive API frame from the UART.
*
*	@param msg pointer to the msg buffer to be populated
*	@param length length of the API frame data 
*
*	@return true if checksum was OK
*/
static bool read_msg(Message* msg, uint16_t length){
	
	
	//length of rf data (aka mac payload)
	msg->data_length = length - 5;
	
	//source address
	msg->address = ((uint16_t)xbee_uart_getc())<<8;
	msg->address += xbee_uart_getc();
	
	//rssi
	msg->rssi = xbee_uart_getc();
	
	//ignore option
	xbee_uart_getc();
	
	for(uint32_t i=0; i<msg->data_length; i++){
		msg->data[i] = xbee_uart_getc();
	}
	
	//discard checkcum... fix this later
	xbee_uart_getc();
	
	return true;
}


/**
*	Read message response
*
*	Reads a message response API frame from the UART.
*
*	@param status pointer to the status variable to be populated
*	@param length length of the API frame data 
*
*	@return true if checksum was OK
*/

static bool read_msg_response(XbeeStatus* status, uint8_t* msg_id, uint16_t length){
	//ignore length... fix this
	
	//frame id
	*msg_id = xbee_uart_getc();
	
	//response
	*status = xbee_uart_getc();
	
	//discard checkcum... fix this later
	xbee_uart_getc();
	
	return true;
}

/**
*
*	Given an API frame with some information it fills the rest 
*	to create a complete API msg frame
*
*	@param api_frame the frame to be filled
*
*/
static void create_msg_frame( ApiFrameMsg *api_frame ){
	
	//set delimiter
	api_frame->start_delimiter = START_DELIMITER;
	
	//calculate checksum
	uint32_t sum = 0;
	
	sum += api_frame->command_id;
	sum += api_frame->frame_id;
	sum += api_frame->dest_address[0];
	sum += api_frame->dest_address[1];
	sum += api_frame->options;

	for( uint32_t i = 0; i < api_frame->rf_data_length ; i++ ) //careful with the minus 1
	sum += api_frame->rf_data[i];
	
	api_frame->checksum = 0xFF - (uint8_t)sum;
}

/**
*	Does the actual sending of an API msg frame
*
*	@param api_frame the frame to be sent
*
*/
static void send_msg_frame( ApiFrameMsg *frame ){
	//in the presence of an OS (with threads) this function should execute atomically
	//Since the Xbee is a shared resource...
	
	//LOCK
	
	//send delimiter and length
	xbee_uart_putc( frame->start_delimiter );
	
	//send length
	xbee_uart_putc( (uint8_t)(frame->length >> 8) );
	xbee_uart_putc( (uint8_t)(frame->length) );
	
	//send cmd id
	xbee_uart_putc( frame->command_id );
	
	//send cmd id
	xbee_uart_putc( frame->frame_id );
	
	//send dest address
#ifdef XBEE_64_ADDR_MODE_ENABLED
	for(uint32_t i=0; i<8; i++){
		xbee_uart_putc( frame->dest_address[i] );
	}
#else
	for(uint32_t i=0; i<2; i++){
		xbee_uart_putc( frame->dest_address[i] );
	}
#endif

	//send options
	xbee_uart_putc( frame->options );
	
	//send rf data
	for(uint32_t i=0; i<frame->rf_data_length; i++){
		xbee_uart_putc( frame->rf_data[i] );
	}
	
	//send checksum
	xbee_uart_putc(  frame->checksum );
	
	//UNLOCK....
}

/**
*
*
*	Given an API frame it fills the rest to create an 
*	AT Command API frame
*
*	@param api_frame the frame to be populated
*
*/
static void create_at_command_frame( ApiFrameATCommand *api_frame ){
	
	//set delimiter
	api_frame->start_delimiter = START_DELIMITER;
	
	//calculate checksum
	uint32_t sum = 0;
	
	sum += api_frame->command_id;
	sum += api_frame->frame_id;
	sum += api_frame->at_command[0];
	sum += api_frame->at_command[1];

	for( uint32_t i = 0; i < api_frame->at_param_length ; i++ ) //careful with the minus 1
		sum += api_frame->at_param[i];
	
	api_frame->checksum = 0xFF - (uint8_t)sum;
}

/**
*	Does the actual sending of an API AT Command frame
*
*	@param api_frame the frame to be sent
*
*/
static void send_at_command_frame( ApiFrameATCommand *frame ){
	//in the presence of an OS (with threads) this function should execute atomically
	//Since the Xbee is a shared resource and this is one single Xbee operatoin...
	
	//LOCK
	
	
	//send delimiter
	xbee_uart_putc( frame->start_delimiter );
	
	//send length
	xbee_uart_putc( (uint8_t)(frame->length >> 8) );
	xbee_uart_putc( (uint8_t)(frame->length));
	
	//send cmd id
	xbee_uart_putc( frame->command_id );
	
	//send frame id
	xbee_uart_putc( frame->frame_id );
	
	//send at command
	xbee_uart_putc( frame->at_command[0] );
	xbee_uart_putc( frame->at_command[1] );
	
	//send parameter value
	for(uint32_t i=0; i< frame->at_param_length; i++){
		xbee_uart_putc( frame->at_param[i] );
	}

	//send checksum
	xbee_uart_putc( frame->checksum );
	
	//unlock
}

/**
*	Converts a given baudrate (e.g 9600) to an Xbee baudrate "param" (i.e. 1, 2 .. 7)
*
*	@return Xbee baudrate param
*
*/
static uint8_t baudrate_to_num(uint32_t value){
	switch(value){
		case 1200: return 0;
		case 2400: return 1;
		case 4800: return 2;
		case 9600: return 3;
		case 19200: return 4;
		case 38400: return 5;
		case 57600: return 6;
		case 115200: return 7;
		default: return 8;
	}
}

/**
*	Checks the Xbee's baudrate is the same as the specified
*
*	@param baudrate the baudrate to be verified
*
*	@return true if baud rates match 
*
*/
static bool is_xbee_baudrate_correct( uint32_t baudrate){
	
	// -- Read baud rate --
	
	xbee_send_at_command( (uint8_t*)"BD", (uint8_t*)"", 0 );

	// -- Read response --
	
	while( xbee_uart_getc() != START_DELIMITER );
	
	//length
	xbee_uart_getc();
	xbee_uart_getc();
	
	//Read API (cmd) Identifier
	xbee_uart_getc();
	
	//frame id
	xbee_uart_getc();
	
	//AT command
	xbee_uart_getc();
	xbee_uart_getc();
	
	//status
	xbee_uart_getc();
	
	//value requested (the baud rate)
	volatile uint8_t br = xbee_uart_getc() << 24;
	br += xbee_uart_getc() << 16;
	br += xbee_uart_getc() << 8;
	br += xbee_uart_getc() << 0;
	
	//checksum
	xbee_uart_getc();
	
	
	// -- compare --
	
	if(br <= 7)
		return br == baudrate_to_num( baudrate );
	else
		return br == baudrate;
}