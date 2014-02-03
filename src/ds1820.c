/*
 * sensor.c
 *
 * Created: 14.6.2011 15:52:19
 *  Author: Lauri
 */ 
#ifndef F_CPU
#define F_CPU 12500000UL
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/crc16.h>

#include "ds1820.h"


//Sensors
char EEMEM _eeprom_sensor_tag1[DS1820_TAG_SIZE] = "Sensor1";
char EEMEM _eeprom_sensor_tag2[DS1820_TAG_SIZE] = "Sensor2";
char EEMEM _eeprom_sensor_tag3[DS1820_TAG_SIZE] = "Sensor3";
char* EEMEM _eeprom_sensor_tags[] = {_eeprom_sensor_tag1, _eeprom_sensor_tag2, _eeprom_sensor_tag3};



int temp[3] = {DS1820_TEMP_NONE, DS1820_TEMP_NONE, DS1820_TEMP_NONE};
int temp_min[3] = {DS1820_TEMP_NONE, DS1820_TEMP_NONE, DS1820_TEMP_NONE};
int temp_max[3] = {DS1820_TEMP_NONE, DS1820_TEMP_NONE, DS1820_TEMP_NONE};


int ds1820_get_cur(int sensor){
	if((sensor >= 0) && (sensor <= 2)){
		return temp[sensor];
	}
	return DS1820_TEMP_NONE;
}

int ds1820_get_min(int sensor){
	if((sensor >= 0) && (sensor <= 2)){
		return temp_min[sensor];
	}
	return DS1820_TEMP_NONE;
}

int ds1820_get_max(int sensor){
	if((sensor >= 0) && (sensor <= 2)){
		return temp_max[sensor];
	}
	return DS1820_TEMP_NONE;
}

void ds1820_reset_minmax(int sensor){
	
	if((sensor >= 0) && (sensor <= 2)){
		temp_min[sensor] = temp[sensor];
		temp_max[sensor] = temp[sensor];
	}
}


int ds1820_print_temp(char* buf, int temp){
    
	if(temp != DS1820_TEMP_NONE){
		if(temp == 0){
			return sprintf_P(buf, PSTR("0.0"));
		}else{
			return sprintf_P(buf, PSTR("%c%d.%d"), (temp < 0)?'-':'+', (abs(temp) / 10), (abs(temp) % 10));
		}
	}else{
		return sprintf_P(buf, PSTR("none"));
	}
    
	return 0;
}

int ds1820_print_tag(char* buf, int sensor){
	
	char c;
	int idx = 0;
	
	if((sensor < 0) && (sensor > 2)){
		return 0;
	}
	
	uint8_t* eeprom_str = (uint8_t *)eeprom_read_word((void*)&_eeprom_sensor_tags[sensor]);
    
	while((c = (char)eeprom_read_byte((const uint8_t*)eeprom_str++)) && (idx < DS1820_TAG_SIZE)){
		buf[idx++] = c;
	}
    
	return idx;
}

int ds1820_set_tag(uint8_t len, const char* tag, int sensor){
	
	if((sensor < 0) && (sensor > 2)){
		return 0;
	}
	
	if(len > DS1820_TAG_SIZE){
		len = DS1820_TAG_SIZE;
	}
	
	uint8_t* eeprom_str = (uint8_t *)eeprom_read_word((void*)&_eeprom_sensor_tags[sensor]);
	
	eeprom_write_block(tag, eeprom_str, len);
	
	if(len < DS1820_TAG_SIZE){
		eeprom_write_byte(eeprom_str + len, 0);
	}
	
	return len;
}


/*Reset DS1820*/
static inline uint8_t ds1820_reset(int sensor){
	
	DS1820_DDR |= (1<<sensor);			//output
	DS1820_PORT &= ~(1<<sensor);		//low
	
	_delay_us(480);						//reset pulse, keep line low
	DS1820_DDR &= ~(1<<sensor);			//input
	_delay_us(60);						//sample between 60-240
	if(DS1820_PIN & (1<<sensor)){		//presence pulse
		return 1;						// no presence
	}
	_delay_us(420);						//total Rx length min 480
	return 0;							//presence detected

}



/*Write one BIT to 1-wire data line*/
static inline void ds1820_write_bit(uint8_t bit, int sensor){

	DS1820_PORT &= ~(1<<sensor);		//low
	DS1820_DDR |= (1<<sensor);			//output
	
	if(bit){
		DS1820_PORT |= (1<<sensor);		//high
	}
	_delay_us(60);						//60-120
	
	DS1820_DDR &= ~(1<<sensor);			//input
	_delay_us(1);						//min delay between write slots
}



/*Read one BIT from 1-wire data line*/
static inline uint8_t ds1820_read_bit(int sensor){

	DS1820_PORT &= ~(1<<sensor);		//low
	DS1820_DDR |= (1<<sensor);			//output
	_delay_us(1);						//minimum 1
	
	DS1820_DDR &= ~(1<<sensor);			//input
	_delay_us(13);
	
	if(DS1820_PIN & (1<<sensor)){
		_delay_us(60);
		return 1;
	}
	_delay_us(60); //delay between read slots
	return 0;
}



/*Write one BYTE to 1-wire data line*/
static inline void ds1820_write_byte(uint8_t byte, int sensor){

	for(int i = 0; i < 8; i++){
		ds1820_write_bit((byte & (1<<i)), sensor);
	}
}



/*Read one BYTE from 1-wire data line*/
static inline void ds1820_read_byte(uint8_t *byte, int sensor){
	
	*byte = 0; //format byte
	
	for(int i = 0; i < 8; i++){
		*byte |= (ds1820_read_bit(sensor) << i);
	}
}



/*Check crc for scratchpad data*/
static inline uint8_t ds1820_crc_check(uint8_t *scratchpad){
	
	uint8_t crc = 0;
	
	for(int i = 0; i < 8; i++)
		crc = _crc_ibutton_update(crc, scratchpad[i]);
	
	if(crc != scratchpad[8]){
		return 1;
	}
	return 0;
}


/*Convert*/
static inline int ds1820_convert(int sensor){
	
	if(ds1820_reset(sensor)){
		return 1;
	}	
	
	ds1820_write_byte(DS1820_CMD_SKIPROM, sensor);
	ds1820_write_byte(DS1820_CMD_CONVERTTEMP, sensor);
	
	return 0;
}



/*0 = not ready, 1 = ready*/
static inline int ds1820_convert_ready(int sensor){
	return ds1820_read_bit(sensor);
}


/**
 * Called from ds1820_refrech_all()
 */
static inline int ds1820_get_temp(int sensor){
	
	uint8_t scratchpad[9];
	int temp_val = 0;
	int temp = 0;
	
	do{
		
		if(ds1820_reset(sensor)){
			return DS1820_TEMP_NONE;
		}
		
		ds1820_write_byte(DS1820_CMD_SKIPROM, sensor);
		ds1820_write_byte(DS1820_CMD_RSCRATCHPAD, sensor);
		
		for(int i = 0; i < 9; i++){							//Read scratchpad
			ds1820_read_byte(&scratchpad[i], sensor);
		}
		
		ds1820_reset(sensor);
		
	}while(ds1820_crc_check(scratchpad));					//while CRC matches
	
	
	//temp = MSB LSB
	temp_val = scratchpad[0];
	temp_val |= (scratchpad[1]<<8);
	temp_val <<= 7;						//keep sign bit
	temp_val &= 0xFF00;					//clear LSB
	temp_val -= 0x40;					//subtract 0.25d = 40h
	temp_val += (((scratchpad[7] - scratchpad[6])<<8) / scratchpad[7]);
	
	temp = temp_val >> 8;
	temp *= 10;
	temp += (((temp_val & 0x00FF) * 100)>>8) / 10;
	
	return temp;

}


/**
 *
 */
void ds1820_refresh_all(){
	
	for(int i = 0; i < 3; i++){
		
		//Read temperatures from sensors whose conversion is ready
		if((temp[i] != DS1820_TEMP_NONE) && (ds1820_convert_ready(DS1820_PIN1 + i))){
			temp[i] = ds1820_get_temp(DS1820_PIN1 + i);
			
			//Check for first values
			if(temp_min[i] == DS1820_TEMP_NONE){
				temp_min[i] = temp[i];
			}
			if(temp_max[i] == DS1820_TEMP_NONE){
				temp_max[i] = temp[i];
			}
			
			//Update min/max values
			if(temp[i] < temp_min[i]){
				temp_min[i] = temp[i];
			}else
			if(temp[i] > temp_max[i]){
				temp_max[i] = temp[i];
			}
		}
		
		//Start conversion for the next call
		if(ds1820_convert(DS1820_PIN1 + i)){
			temp[i] = DS1820_TEMP_NONE; //Indication for ready check that there is no sensor
			//temp_min[DS1820_PIN1 + i] = DS1820_TEMP_NONE;	//Reset min/max values
			//temp_max[DS1820_PIN1 + i] = DS1820_TEMP_NONE;
		}else
		if(temp[i] == DS1820_TEMP_NONE){
			temp[i] = DS1820_TEMP_INIT;	//Indication for ready check that there is a sensor
		}
		
	}

}
