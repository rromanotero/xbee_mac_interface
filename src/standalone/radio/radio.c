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
 * @file	radio.c
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief Radio abstraction
 *
 * Wraps up the Xbee library into a radio abstraction.
 *
 */

#include <stdint-gcc.h>
#include <stdbool.h>
#include "xbee/xbee_cpu.h"
#include "xbee/xbee.h"
#include "radio.h"
#include "mac_config.h"

static void at_command_response(XbeeATCommandResponse*);							
static void blocking_send_at_command(const uint8_t*, const uint8_t*, uint8_t );	
static uint16_t fix_endianness_16bit(uint16_t original);

static volatile XbeeATCommandResponse response; //One reponse for all upper layer threads (hence, shared memory)
static volatile bool waiting_for_response = false;

/**
*	Initializes the radio.
*
*	Checks the radio speed rate matches RADIO_SPEED_RATE and initializes the radio. 
*	Takes ~2secs to complete. If radio not present execution halts here.
*
*	@return true if speed rates matched, false otherwise
*/
bool radio_init(void){
	
	//registers event handler
	xbee_register_at_command_responded_callback(at_command_response);
	
	
	return xbee_init(RADIO_SPEED_RATE);  //takes ~2 secs to complete
}

/**
*	Radio read 16-bit address.
*
*	Reads the 16-bit address of the radio.
*
*	@return the address
*/
uint16_t radio_read_16bit_address(void){
	//LOCK OR MUTEX.... Fix this
	
	//send command
	blocking_send_at_command( (uint8_t*)"MY", (uint8_t*)"", 0 );

	//extract numeric value
	uint16_t address = response.value_requested[0]*256 + response.value_requested[1];
	
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return address;//later fix... check that status == OK (0) and not error
}

/**
*	Radio read PAN ID.
*
*	Reads the PAN ID of the radio.
*
*	@return the PAN ID
*/
uint16_t radio_read_panid(void){
		//LOCK OR MUTEX.... Fix this
		
		//send command
		blocking_send_at_command( (uint8_t*)"ID", (uint8_t*)"", 0 );

		//extract numeric value
		uint16_t panid = response.value_requested[0]*256 + response.value_requested[1];
		
		//UNLOCK OR RELEASE MUTEX.... Fix this

		return panid;//later fix... check that status == OK (0) and not error
}

/**
*	Radio read channel.
*
*	Reads the channel where the radio is transmitting
*
*	@return the channel
*/
uint8_t radio_read_channel(void){
	//LOCK OR MUTEX.... Fix this
	
	//send command
	blocking_send_at_command( (uint8_t*)"CH", (uint8_t*)"", 0 );

	//extract numeric value
	uint8_t channel = response.value_requested[0];
	
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return channel;//later fix... check that status == OK (0) and not error
}

/**
*	Radio read extra retries.
*
*	reads how many "extra retries" the radio uses in
*	addition to the 3 provided by the IEEE 802.15.4 MAC.
*	For each extra retry specified here the radio can execute
*	up to 3 retries.
*
*	@return # of retries, maximum 6
*/
uint8_t radio_read_extra_retries(void){

	//LOCK OR MUTEX.... Fix this
	
	//send command
	blocking_send_at_command( (uint8_t*)"RR", (uint8_t*)"", 0 );

	//extract numeric value
	uint8_t retries = response.value_requested[0];
	
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return retries;//later fix... check that status == OK (0) and not error

}

/**
*	Radio read Tx power
*
*	Reads the power level at which the radio transmits
*	0 = -10dBm, 1 = -6dBm, 2 = -4dBm, 3 = -2dBm, 4 = 0dBm
*
*	@return the tx power level
*/
uint8_t radio_read_tx_power(void){
	//LOCK OR MUTEX.... Fix this
	
	//send command
	blocking_send_at_command( (uint8_t*)"PL", (uint8_t*)"", 0 );

	//extract numeric value
	uint8_t power = response.value_requested[0];
	
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return power;//later fix... check that status == OK (0) and not error
}

/**
*	Radio read CCA Threshold
*
*	Reads the Clear Channel Assesment Threshold. Prior to 
*	transmitting a package a CCA is performed. The transmission 
*	will continue if the energy level on the channel is lower than
*	the specified threshold.
*	Min = 0x24 -dBm, Max = 0x50 -dBm.
*	
*	@return the CCA threshold in -dBm
*/
uint8_t radio_read_cca_threshold(void){
	//LOCK OR MUTEX.... Fix this
	
	//send command
	blocking_send_at_command( (uint8_t*)"CA", (uint8_t*)"", 0 );

	//extract numeric value
	uint8_t threshold = response.value_requested[0];
	
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return threshold;//later fix... check that status == OK (0) and not error
}

/**
*	Radio read acks
*
*	Reads whether the radio uses packet acknowledgement or not
*	in transmissions
*
*	@return acks activated, yes or not
*/
bool radio_read_acks(void){
	//LOCK OR MUTEX.... Fix this
	
	//send command
	blocking_send_at_command( (uint8_t*)"MM", (uint8_t*)"", 0 );

	//extract numeric value
	uint8_t v = response.value_requested[0];
	
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return v;//later fix... check that status == OK (0) and not error
}

/**
*	Radio read macMinBE.
*
*	Reads the minimum back off exponent (or random delay slots) of the CSMA-CA algorithm.
*
*	@return The macMinBEE (between 0 and 3)
*/
uint8_t radio_read_macminbe(void){
	
	//LOCK OR MUTEX.... Fix this
		
	//send command
	blocking_send_at_command( (uint8_t*)"RN", (uint8_t*)"", 0 );

	//extract numeric value
	uint8_t value = response.value_requested[0];
		
	//UNLOCK OR RELEASE MUTEX.... Fix this

	return value;//later fix... check that status == OK (0) and not error

}


/**
*	Radio write 16-bit address.
*
*	Changes the radio 16-bit address to the specified one.
*
*	@param value the address
*/
void radio_write_16bit_address(uint16_t value){
	
	uint16_t fixed_value = fix_endianness_16bit(value);	
	
	blocking_send_at_command( (uint8_t*)"MY", (uint8_t*)(&fixed_value), 2 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  						//write changes to nonvolatile
}

/**
*	Radio write channel.
*
*	Changes the radio channel to the specified one. Input must be
*	a valid IEEE 802.15.4 channel.
*
*	@param value the channel
*/
void radio_write_channel(uint8_t value){
	
	if(value < 0x0B || value > 0x1A)
		return;
		
	blocking_send_at_command( (uint8_t*)"CH", (uint8_t*)(&value), 1 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  					//write changes to nonvolatile
}

/**
*	Radio write acks.
*
*	Tells the radio whether to use packet acknowledgment or not 
*	in transmissions.
*
*	@param acks_allowed yes or not
*/
void radio_write_acks(bool acks_allowed){
	
	uint8_t mac_mode;
	
	if(acks_allowed) mac_mode = 2; //ieee 802.15.4 with acks
	else mac_mode = 1;				//ieee 802.15.4 no acks

	blocking_send_at_command( (uint8_t*)"MM", (uint8_t*)(&mac_mode), 1 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  		//write changes to nonvolatile
}


/**
*	Radio write PAN ID.
*
*	Changes the radio's PAN ID to the specified one.
*
*	@param pan_id the PAN ID
*/
void radio_write_panid(uint16_t pan_id){
	
	uint16_t fixed_value = fix_endianness_16bit(pan_id);
	
	blocking_send_at_command( (uint8_t*)"ID", (uint8_t*)(&fixed_value), 2 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  						//write changes to nonvolatile
}

/**
*	Radio write retries.
*
*	(NOT WORKING) Tells the radio how many "extra retries: to use in 
*	addition to the 3 provided by the IEEE 802.15.4 MAC. 
*	For each extra retry specified here the radio can execute
*	up to 3 retries.
*
*	@param retries # of retries, maximum 6
*/
void radio_write_extra_retries(uint8_t retries){

	if(retries > 6)	
		return;
		
	blocking_send_at_command( (uint8_t*)"RR", (uint8_t*)(&retries), 1 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  		//write changes to nonvolatile
}

/**
*	Radio Tx power.
*
*	Sets the power level at which the radio transmits
*	0 = -10dBm, 1 = -6dBm, 2 = -4dBm, 3 = -2dBm, 4 = 0dBm.
*
*	@param power the TX power level
*/
void radio_write_tx_power(uint8_t power){

	if(power > 4)
		return;
	
	blocking_send_at_command( (uint8_t*)"PL", (uint8_t*)(&power), 1 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  		//write changes to nonvolatile
}

/**
*	Radio write CCA Threshold.
*
*	Writes the Clear Channel Assessment Threshold. Prior to
*	transmitting a package a CCA is performed. The transmission
*	will continue if the energy level on the channel is lower than
*	the specified threshold. 
*	Min = 0x24 -dBm, Max = 0x50 -dBm.
*
*	@param threshold the threshold in -dBm
*/
void radio_write_cca_threshold(uint8_t threshold){

	if(threshold < 0x24 || threshold > 0x50)
		return;
	
	blocking_send_at_command( (uint8_t*)"CA", (uint8_t*)(&threshold), 1 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  		//write changes to nonvolatile
}
/**
*	Radio write macMinBE.
*
*	Sets the minimum back off exponent (or random delay slots) of the 
*	CSMA-CA algorithm. The value must be between 0 and 3.
*
*	@param value the desired value
*/
void radio_write_macminbe(uint8_t value){
	
	if( value > 3 || value < 0 )
		return;
		
	blocking_send_at_command( (uint8_t*)"RN", (uint8_t*)(&value), 1 );	//send command
	blocking_send_at_command( (uint8_t*)"WR", (uint8_t*)"", 0 );  					//write changes to nonvolatile
}


///////////////////////////////////////////////////////////////////////////////////////////////////
////////////                                   L  O  C  A  L                            //////////
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
*	AT Command response event
*
*	This is how the lower layer (Xbee library) notifies of an AT command response received.
*
*	@param response the response
*/
static void at_command_response(XbeeATCommandResponse* r){
	
	//copies response in buffer
	response.status = r->status;
	response.value_requested_length = r->value_requested_length;
	
	for( uint8_t i=0; i< response.value_requested_length; i++ )
	response.value_requested[i] = r->value_requested[i];
	
	//signals data is ready
	waiting_for_response = false;
}

/**
*	Fix endianness 16-bit.
*
*	Given a 16-bit value with either endianness, it return the same value
*	in big-endian ordering
*
*	@param value the value as big endian
*/
static uint16_t fix_endianness_16bit(uint16_t original){
	//Xbee arranges (in frames) the order of multibyte values as big endian
	//So if the target platform is little endian this function changes it
		
	if( xbee_cpu_is_little_endian() )
		return xbee_cpu_swap_endianness_16bit(original);
	
	return original;
}


/**
*	Blocking send AT command.
*
*	The underlying Xbee library is interrupt based, so this 
*	creates a blocking function that sends an AT command and 
*	returns when a response has being received.
*
*/
static void blocking_send_at_command(const uint8_t* command, const uint8_t* params, uint8_t params_length){
	
	//we will be waiting for incoming command response
	waiting_for_response = true;
	
	//sends command
	xbee_send_at_command( command, params, params_length);
	
	//waits until response has being received
	//use semaphore instead when OS is present.... response.wait();...  FIX THIS....
	while(waiting_for_response);
}
