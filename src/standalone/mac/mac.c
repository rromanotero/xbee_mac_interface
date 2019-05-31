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
 * @file	mac.c
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief IEEE 802.15.4 MAC interface.
 *
 * This interface presents the IEEE 802.15.4 MAC layer
 * functionality implemented in hardware (the Xbee) as an actual MAC interface. It implements
 * message and frame send/receive abstractions.
 */

#include <stdint-gcc.h>
#include <stdbool.h>
#include "xbee/xbee.h"
#include "radio/radio.h"
#include "mac.h"
#include "mac_config.h"


//Xbee to MAC Callbacks
static void msg_received(Message*);				///< Xbee-to-MAC messasge received callback
static void msg_response(XbeeStatus, uint8_t);	///< Xbee-to-MAC msg response received callback

//MAC to App callbacks
static void (*app_msg_received_callback)(Message*);	///< MAC-to-upper-layer message received callback
static void (*app_ack_received_callback)(uint8_t);	///< MAC-to-upper-layer ack received callback
							
static uint8_t dummy_id = 7;	



/**
*	MAC Init.
*
*	Initializes the IEEE 802.25.4 MAC. 
*
*	@param msg_callback When a msg is received the registered msg_callback is called
*	@param ack_callback When an ack is received the registered ack_callback is called
*
*	@return true if communication with radio was possible and stored speed rate matches RADIO_SPEED_RATE 
*/
bool mac_init( void(*msg_callback)(Message* msg), void(*ack_callback)(uint8_t) ){
	
	//initializes radio
	if( !radio_init() ) return false;
	
	//initializes app callbacks (from mac to upper layer)
	app_msg_received_callback = msg_callback;
	app_ack_received_callback = ack_callback;
	
	//registers callbacks (from lower layer to mac) 
	xbee_register_msg_received_callback(msg_received);
	xbee_register_msg_responded_callback(msg_response);
	
	//sets up radio and mac parameters as per the mac_config file
	radio_write_16bit_address(MAC_ADDRESS);
	radio_write_channel(MAC_CHANNEL);
	radio_write_panid(MAC_PAN_ID);
	radio_write_macminbe(MAC_macMinBE);
	radio_write_acks(MAC_ACKS);
	//radio_write_extra_retries(MAC_EXTRA_RETRIES); //NOT WORKING!... fix this
	radio_write_tx_power(RADIO_TX_POWER);
	radio_write_cca_threshold(RADIO_CCA_THRESHOLD);	
	
	return true;
}

/**
*	Send message.
*
*	Send an IEEE 802.25.4 MAC message.
*
*	@param msg the message 
*/
void mac_send( Message* msg ){
	xbee_send_msg( msg, dummy_id );
}


/**
*	Msg received event
*
*	This is how the lower layer notifies the MAC interface of message received.
*	In turn the callback(event) from the upper layer is called
*
*	@param msg the message received
*/
static void msg_received(Message *msg){
	//msg.lqi = ...
	(*app_msg_received_callback)(msg);
}

/**
*	Msg response event
*
*	This is how the lower layer notifies the MAC interface of message response received.
*	In turn the callback(event) from the upper layer is called
*
*	@param msg the message status
*	@param dummy_msg_id dummy id associated with the msg being acked
*/
static void msg_response(XbeeStatus msg_status, uint8_t dummy_msg_id){
	(*app_ack_received_callback)(msg_status);
}

