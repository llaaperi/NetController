/*
 * twi.c
 *
 * Created: 11.6.2011 1:57:45
 *  Author: Lauri
 */ 

#include "twi.h"

void twi_init(){
					//Set two-wire serial interface clock frequency
					//SCL frequency is ~100kHz with 1x prescaling									
	//TWBR = 24;	//F_CPU 6250000					
	TWBR = 55;		//F_CPU 12500000
}

void twi_write_byte(char addr, char byte){
	
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);		//START
	while(!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8) != TW_START){					//Error check
		//lcd_write_hex(TWSR & 0xF8);
	}
	
	TWDR = addr | TW_WRITE;							//SLA_W
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8) != TW_MT_SLA_ACK){				//Error check
		//lcd_write_hex(TWSR & 0xF8);
	}
	
	TWDR = byte;									//Write byte
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8) != TW_MT_DATA_ACK){			//Error check
		//lcd_write_hex(TWSR & 0xF8);
	}
	
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);		//END
	
}

char twi_read_byte(char addr, char* byte){
	
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);		//START
	while(!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8) != TW_START){					//Error check
		//lcd_write_hex(TWSR & 0xF8);
		//state(ERROR);
		//return 1;
	}
	
	TWDR = addr | TW_READ;							//SLA_R
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8) != TW_MR_SLA_ACK){				//Error check
		//lcd_write_hex(TWSR & 0xF8);
		//state(ERROR);
		//return 1;
	}
	
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));					//Wait for byte
	if((TWSR & 0xF8) != TW_MR_DATA_NACK){			//Error check
		//lcd_write_hex(TWSR & 0xF8);
		//state(ERROR);
	}

	*byte = TWDR;									//Read byte

	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);		//STOP
	
	return 0;
}