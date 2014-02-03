/*
 * ds1820.h
 *
 * Created: 14.6.2011 15:52:02
 *  Author: Lauri
 */ 


#ifndef DS1820_H
#define DS1820_H

/*DS1820 connections*/
#define DS1820_DDR DDRD
#define DS1820_PORT PORTD
#define DS1820_PIN PIND
#define DS1820_PIN1 PD5
#define DS1820_PIN2 PD6
#define DS1820_PIN3 PD7


/* DS1820 function commands*/
#define DS1820_CMD_CONVERTTEMP 0x44
#define DS1820_CMD_RSCRATCHPAD 0xBE
#define DS1820_CMD_WSCRATCHPAD 0x4E
#define DS1820_CMD_CPYSCRATCHPAD 0x48
#define DS1820_CMD_RECEEPROM 0xB8
#define DS1820_CMD_RPWRSUPPLY 0xB4
#define DS1820_CMD_SEARCHROM 0xF0
#define DS1820_CMD_READROM 0x33
#define DS1820_CMD_MATCHROM 0x55
#define DS1820_CMD_SKIPROM 0xCC
#define DS1820_CMD_ALARMSEARCH 0xEC

#define DS1820_TEMP_NONE 2000
#define DS1820_TEMP_INIT 2001

#define DS1820_TAG_SIZE 16

#include <stdint.h>


int ds1820_get_cur(int sensor);
int ds1820_get_min(int sensor);
int ds1820_get_max(int sensor);
void ds1820_reset_minmax(int sensor);
int ds1820_print_temp(char* buf, int temp);

int ds1820_set_tag(uint8_t len, const char* tag, int sensor);
int ds1820_print_tag(char* buf, int sensor);

void ds1820_refresh_all();

#endif /* DS1820_H */