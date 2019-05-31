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
 * @file	xbee_cpu.h
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief header file for xbee_cpu.c
 *
 */


#ifndef XBEE_CPU_H_
#define XBEE_CPU_H_

bool xbee_cpu_is_little_endian(void);
uint16_t xbee_cpu_swap_endianness_16bit(uint16_t);
void xbee_cpu_delay_ms(uint32_t);


#endif /* XBEE_CPU_H_ */