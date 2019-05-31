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
 * @file	xbee_uart.h
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief header file for xbee_uart.c
 *
 */


#ifndef XBEE_UART_H_
#define XBEE_UART_H_

void xbee_uart_enable_interrupt(void);
void xbee_uart_disable_interrupt(void);
uint8_t xbee_uart_getc(void);
void xbee_uart_putc(uint8_t);
void xbee_uart_puts(const uint8_t*);
void xbee_uart_config_init(uint32_t);
void xbee_uart_register_callback( void(*)(void) );	

#endif 