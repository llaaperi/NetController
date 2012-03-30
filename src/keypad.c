/*
 * keypad.c
 *
 * Created: 11.6.2011 2:28:08
 *  Author: Lauri
 */ 

#include "keypad.h"

volatile char key_flag = 0;


void keypad_init(){
	
	twi_write_byte(KEYPAD_ADDR, 0x0F);				//Write all rows high
	
	DDRD &= ~(1<<2);								//Interrupt pin as input
	PORTD |= (1<<2);								//Activate internal pull-up
	EICRA |= (1<<ISC01);							//Interrupt INT0 on falling edge
	//EICRA &= ~((1<<ISC01) & (1<<ISC00));			//Interrupt INT0 on low level
	EIMSK |= (1<<INT0);								//Enable external interrupt INT0
}

char keypad_get_key(){

    uint8_t byte = 0, tmp = 0;
	char key = 0;
    
	_delay_ms(5);							//Short delay to prevent bouncing when key is pressed
	
	twi_write_byte(KEYPAD_ADDR, 0x0F);		//Write all rows high
	twi_read_byte(KEYPAD_ADDR, &tmp);
	byte |= ~(0xF0 | tmp);					//Read column
	
	twi_write_byte(KEYPAD_ADDR, 0xF0);		//Write all columns high
	twi_read_byte(KEYPAD_ADDR, &tmp);
	byte |= ~(0x0F | tmp);					//Read row
	
	twi_write_byte(KEYPAD_ADDR, 0x0F);		//Return to original state
	
	switch(byte){							//Map key value to button
		case 0x88: key = '1'; break;
		case 0x84: key = '4'; break;
		case 0x82: key = '7'; break;
		case 0x81: key = '*'; break;
		case 0x48: key = '2'; break;
		case 0x44: key = '5'; break;
		case 0x42: key = '8'; break;
		case 0x41: key = '0'; break;
		case 0x28: key = '3'; break;
		case 0x24: key = '6'; break;
		case 0x22: key = '9'; break;
		case 0x21: key = '#'; break;
		case 0x18: key = 'A'; break;
		case 0x14: key = 'B'; break;
		case 0x12: key = 'C'; break;
		case 0x11: key = 'D'; break;
		default: key = 0;					//Error value is NULL
	}
	
	return key;
	
}


char keypad_get(){
	
	char key = 0;
	
	if(key_flag){
		key_flag = 0;
		
		_delay_ms(20);					//"Small" delay to let port states stabilize 
		key = keypad_get_key();
	}
	
	return key;
}


ISR(INT0_vect){
	key_flag = 1;
}
