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
 * @file	xbee_cpu.c
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief Low-level CPU routines
 *
 * When porting the Xbee library 
 * this module has to be rewritten.
 */

#include <asf.h>
#include <stdint-gcc.h>
#include <stdbool.h>
#include "xbee_cpu.h"



/**
*	CPU endianness
*
*	Determines the endianness of the CPU
*
*	@return XBEE_CPU_LITTLE_ENDIAN if the CPU is little endian, XBEE_CPU_LITTLE_ENDIAN otherwise
*/
bool xbee_cpu_is_little_endian(void) {
	uint16_t i = 1;
	uint8_t *p = (uint8_t *)&i;

	if (p[0] == 1)
		return true;
	else
		return false;
}

/**
*	Swap endianness 16 bit
*
*	Converts a 16-bit little endian value to big endian and vice versa
*
*	@param value the original value
*
*	@return the swapped value
*/
uint16_t xbee_cpu_swap_endianness_16bit(uint16_t value) {
	uint8_t c1, c2;
	
	c1 = value & 0xff;
	c2 = (value >> 8) & 0xff;
	
	return (c1 << 8) + c2;
	
}



/**
*	Delays Routine in milliseconds.
*
*	Busy waiting delay routine in milliseconds.
*
*	@param time_ms self explanatory
*/
void xbee_cpu_delay_ms(uint32_t time_ms){
	delay_ms(time_ms);
}

