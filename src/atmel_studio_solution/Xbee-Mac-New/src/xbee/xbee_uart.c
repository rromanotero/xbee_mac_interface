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
 * @file	xbee_uart.c
 * @author  Rafael Roman Otero
 * @version 2.0
 *
 * @brief Low-level UART routines.
 *
 * They're used by upper layers to handle 
 * all Xbee communications over the UART. When porting the Xbee library
 * this module has to be rewritten.
 *
 * In the SAM4S Xpro Communication is via USART1 (In pins Rx and Tx in the IO Xplained Pro board).
 * No synchronization pins. Don't forget to power the Xbee via Vcc and Gnd pins
 *
 */

#include <asf.h>
#include <stdint-gcc.h>
#include <stdbool.h>
#include "xbee_uart.h"

#define USART_SERIAL                 USART1
#define USART_SERIAL_ID              ID_USART1
#define USART_SERIAL_CHAR_LENGTH     US_MR_CHRL_8_BIT
#define USART_SERIAL_PARITY          US_MR_PAR_NO
#define USART_SERIAL_STOP_BIT        US_MR_NBSTOP_1_BIT

#define PINS_USART1_PIO      PIOA
#define PINS_USART1_ID       ID_USART1
#define PINS_USART1_TYPE     PIO_PERIPH_A
#define PINS_USART1_ATTR     PIO_DEFAULT
#define PINS_USART1_MASK     (PIO_PA21A_RXD1| PIO_PA22A_TXD1 )

static void (*data_received_callback)(void);					///< UART1 interrupt callback


/**
*	Enables UART interrupt.
*
*	Enables UART interrupt.
*/
void xbee_uart_enable_interrupt(void){
	usart_enable_interrupt(USART_SERIAL, US_IER_RXRDY);
	NVIC_EnableIRQ(USART1_IRQn);
}

/**
*	Disables UART interrupt.
*
*	Disables UART interrupt.
*/
void xbee_uart_disable_interrupt(void){
	usart_disable_interrupt(USART_SERIAL, US_IER_RXRDY);
	NVIC_EnableIRQ(USART1_IRQn);
}

/**
*	putc for UART
*
*	Writes one character to the UART1. Blocks until writing to Tx is possible.
*
*	@param c the character to be written
*/
void xbee_uart_putc(uint8_t c){
	//wait for tx to be ready
	while( !(USART_SERIAL->US_CSR & US_CSR_TXRDY) );
	//write
	usart_write(USART_SERIAL, c);
}

/**
*	getc for UART
* 
*	Reads one character from the UART. Blocks until a character is ready.
*
*	@return A character from the UART buffer
*/
uint8_t xbee_uart_getc(void){
	uint8_t c;
	
	//wait for character
	uint32_t dw_status;
	do{
		dw_status = usart_get_status(USART1);
	}while ( !(dw_status & US_CSR_RXRDY) );
	
	//read character
	usart_read(USART1, &c);
	
	return c;
}

/**
*	puts for UART
*
*	Writes a null-terminated string to the UART. Blocks until transmitting is possible.
*
*	@param buffer A pointer to the null-terminated string
*/
void xbee_uart_puts( const uint8_t* buffer){
	
	while( *buffer ){ //while not null characters
		while( !(USART_SERIAL->US_CSR & US_CSR_TXRDY) ); //wait for tx to be ready
		usart_write(USART_SERIAL, *buffer); //write
		buffer++;									//next character
	}
}

/**
*	Initializes and configures the UART.
*
*	@param baudrate baud rate
*/
void xbee_uart_config_init(uint32_t baudrate){
	pio_configure(PINS_USART1_PIO, PINS_USART1_TYPE, PINS_USART1_MASK, PINS_USART1_ATTR);
	pmc_enable_periph_clk(ID_USART1);

	const sam_usart_opt_t usart_console_settings = {
		baudrate,
		USART_SERIAL_CHAR_LENGTH,
		USART_SERIAL_PARITY,
		USART_SERIAL_STOP_BIT,
		US_MR_CHMODE_NORMAL
	};

	sysclk_enable_peripheral_clock(USART_SERIAL_ID);	
	usart_init_rs232(USART_SERIAL, &usart_console_settings, sysclk_get_peripheral_hz());
	usart_enable_tx(USART_SERIAL);
	usart_enable_rx(USART_SERIAL);
}

/**
*	Registers UART's data received callback
*
*	Allows to register a function that will get call when
*	there is incoming data from UART Rx 
*
*	@param callback a pointer to the callback function
*/
void xbee_uart_register_callback( void(*callback)(void) ){
	data_received_callback = callback;
}

/**
*	USART1 Handler
*
*	USART1 Handler. Where everything begins... 
*/
void USART1_Handler(void){
	
	uint32_t dw_status = usart_get_status(USART1);
	
	if (dw_status & US_CSR_RXRDY) {
		(*data_received_callback)(); //notifies the upper layer data was received
	}
}


