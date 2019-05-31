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
* RSSI example
*
* Node blinks the boards LED, and sends a message every now and then. Blinking rate
* depends on the RSSI of the last message
*/

#include <asf.h>
#include "mac/mac.h"
#include "radio/radio.h"
#include "message.h"

#define ADDRESSEE_NODE		2
#define LED_ON				false

void msg_received(Message *msg);
void ack_received(uint8_t status);
void task_msgsend(void);
void task_ledblink(void);

static uint32_t blink_wait = 2000; //2 secs initial wait

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
	
	while(1){
		task_msgsend();
		task_ledblink();
		
		delay_ms(blink_wait);
	}
}

void task_msgsend(void){
	//sends a message every now and then
	Message msg;
	msg.address = ADDRESSEE_NODE;			//Addressee node
	msg.data[0] = 7;						//send anything (random value)
	msg.data_length = 1;					//we're sending one byte
	mac_send(&msg);
}

static bool led_state = LED_ON;

void task_ledblink(void){
	//blinks led. Blink rate depends on rssi received
	ioport_set_pin_level( IOPORT_CREATE_PIN(PIOC, 23), led_state = !led_state );
}



void msg_received(Message *msg_in){
	//updates the blink wait time, based on the RSSI of msg received
	blink_wait = msg_in->rssi * 18 - 700;
}

void ack_received(uint8_t status){}