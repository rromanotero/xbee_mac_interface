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
* Unicast example
*
* Sends a message every half a second. Toggles the board's LED when
* a message is received
*/

#include <asf.h>
#include "mac/mac.h"
#include "radio/radio.h"
#include "message.h"

#define ADDRESSEE_NODE		2
#define LED_ON				false

void msg_received(Message *msg);
void ack_received(uint8_t status);

Message msg;

void main(void)
{
	sysclk_init();
	board_init();
	
	//Init MAC
	if( !mac_init( msg_received, ack_received ) ){
		//Error: Xbee's baud rate is different to the one specified in mac_config.h (RADIO SPEED RATE)
		while(1);
	}
	
	//signal initialization went fine.
	ioport_set_pin_level( IOPORT_CREATE_PIN(PIOC, 23), LED_ON );
	
	//sends a 1-byte message every half seconds
	while(1){
		msg.address = ADDRESSEE_NODE;		//Addressee node
		msg.data[0] = 7;						//send anything (random value)
		msg.data_length = 1;					//we're sending one byte
		mac_send(&msg);
		
		delay_ms(500);
	}
}

static bool led_state = LED_ON;

void msg_received(Message *msg_in){
	//toggle LED
	ioport_set_pin_level( IOPORT_CREATE_PIN(PIOC, 23), led_state = !led_state );
}

void ack_received(uint8_t status){}