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
 * @file	radio.h
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief header file for radio.c
 * 
 */


#ifndef RADIO_H_
#define RADIO_H_

#define	RADIO_DEFAULT_CCA_THRESHOLD 0x2C	///< Default CCA threshold
#define	RADIO_MAX_SPEED_RATE		57600	///< Max baud rate
#define	RADIO_MAX_TX_POWER			4		///< Max TX Power

bool radio_init(void);
uint16_t radio_read_16bit_address(void);
uint16_t radio_read_panid(void);
uint8_t radio_read_channel(void);
uint8_t radio_read_tx_power(void);
uint8_t radio_read_cca_threshold(void);
uint8_t radio_read_extra_retries(void);
bool radio_read_acks(void);
uint8_t radio_read_macminbe(void);
void radio_write_16bit_address(uint16_t);
void radio_write_panid(uint16_t);
void radio_write_channel(uint8_t);
void radio_write_acks(bool);
void radio_write_tx_power(uint8_t);
void radio_write_cca_threshold(uint8_t);
void radio_write_extra_retries(uint8_t); //not working
void radio_write_macminbe(uint8_t); 


#endif /* RADIO_H_ */