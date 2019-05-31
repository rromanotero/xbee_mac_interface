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
 * @file	xbee.h
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief header file for xbee.c
 *
 */


#ifndef XBEE_H_
#define XBEE_H_

#include "message.h"

#define XBEE_MAX_AT_COMMAND_RESPONSE_LENGTH	8	///< Max length possible of an AT Command response

typedef uint32_t XbeeStatus;	///< Xbee Status 

typedef struct{ ///< AT Command response API frame
	uint8_t status;													///< Status 
	uint8_t value_requested[XBEE_MAX_AT_COMMAND_RESPONSE_LENGTH];	///< Value requested
	uint8_t value_requested_length;									///< Length of value requested
}XbeeATCommandResponse;			


uint32_t xbee_init(uint32_t);
void xbee_send_msg(Message*, uint8_t);
void xbee_send_at_command( const uint8_t*, const uint8_t*, uint8_t );
void xbee_register_msg_received_callback( void (*)(Message*) );
void xbee_register_at_command_responded_callback( void (*)(XbeeATCommandResponse*) );
void xbee_register_msg_responded_callback( void(*)(XbeeStatus, uint8_t) );

#endif /* XBEE_H_ */