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
 * @file	mac.h
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief header file for mac.c
 *
 */

#ifndef MAC_H_
#define MAC_H_

#include "message.h"

#define MAC_DEFAULT_macMinBE 0 ///< Default macMinBE threshold	

bool mac_init( void(*)(Message*), void(*)(uint8_t) );
void mac_send( Message* );

#endif /* MAC_H_ */