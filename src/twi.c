/*
 * twi.c
 *
 * Created: 11.6.2011 1:57:45
 *  Author: Lauri
 */ 

#include <util/twi.h>
#include "twi.h"


/*
 *
 */
void twi_init(uint32_t f_cpu){
	
	//Set two-wire serial interface clock frequency
	//SCL frequency is ~100kHz with 1x prescaling									
	switch (f_cpu) {
		case 6250000:	//F_CPU 6250000
			TWBR = 24;
			break;
		case 12500000:	//F_CPU 12500000
			TWBR = 55;
			break;
		default:
			break;
	}
}


/*
 *
 */
void twi_write_byte(uint8_t addr, uint8_t byte){
	
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


/*
 *
 */
int twi_read_byte(uint8_t addr, uint8_t* byte){
	
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