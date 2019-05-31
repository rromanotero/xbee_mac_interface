/*
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
 * @file	mac_config.h
 * @author  Rafael Roman Otero
 * @version 1.0
 *
 * @brief Configuration file.
 *
 * Set parameters for radio and mac interface here.
 *
 */


#ifndef MAC_CONFIG_H_
#define MAC_CONFIG_H_

#include "mac/mac.h"
#include "radio/radio.h"

//  C U S T O M I Z E    A P P L I C A T I O N    H E R E 
//  Instructions: 
//		1) reset the radio to factory settings
//		2) change radio to API mode
//		3) set the Xbee's baud rate to match RADIO_SPEED_RATE 
//		4) Write changes no non-volatile! (the Xbee library assumes all this is true)

#define MAC_ADDRESS			2							///< Source address
#define MAC_PAN_ID			0xAA						///< Pan ID
#define MAC_CHANNEL			0x11						///< Channel (range: 0x0B - 0x1A)
#define MAC_ACKS			true						///< acknowledge packets?
#define MAC_macMinBE		MAC_DEFAULT_macMinBE		///< Defines macMinBE (minimum back off exponent)
//#define MAC_EXTRA_RETRIES	0							///< (NOT WORKING) extra retries (see Xbee documentation)
#define RADIO_SPEED_RATE	RADIO_MAX_SPEED_RATE		///< UART baud rate. Match it with Xbee's baud rate. (options: 1200, 2400, ... 57600)
#define RADIO_TX_POWER		0							///< 0 = -10dBm, 1 = -6dBm, 2 = -4dBm, 3 = -2dBm, 4 = 0dBm							
#define RADIO_CCA_THRESHOLD RADIO_DEFAULT_CCA_THRESHOLD	///< CCA Energy level Threshold in-dBm. Min = 0, Max = 0x50 
	
#endif /* MAC_CONFIG_H_ */