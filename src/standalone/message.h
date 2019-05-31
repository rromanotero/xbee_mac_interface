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
 * @file	message.h
 * @author  Rafael Roman Otero
 * @version 1.0
 *
 * @brief Contains the definitions for IEEE 802.15.4 messages.
 *
 */


#ifndef MESSAGE_H_
#define MESSAGE_H_

#define MSG_LENGTH	10					///< Message (MAC payload) Length. Customize for every application.
#define MSG_BROADCAST_ADDRESS 0xFFFF	///< Broadcast address
#define MSG_ACK_RECEIVED	0			///< Message ack value
#define MSG_ACK_TIMEOUT		1			///< Message ack value
#define MSG_ACK_CCA_FAILURE	2			///< Message ack value
#define MSG_ACK_PURGED		3			///< Message ack value

typedef struct{		///< The message data structure
	uint16_t address;			  ///< source or destination address (depending on whether the message is being sent or received)
	uint8_t data[MSG_LENGTH];	  ///< MAC payload
	uint8_t	data_length;		  ///< length of MAC payload
	uint8_t rssi;				  ///< rssi associated
}Message;	

#endif /* MESSAGE_H_ */