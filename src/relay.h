/*
 * relay.h
 *
 * Created: 15.6.2011 16:31:19
 *  Author: Lauri
 */ 


#ifndef RELAY_H
#define RELAY_H

#include <avr/eeprom.h>
#include "NetController.h"
#include "ds1820.h"
#include "twi.h"
#include "lcd.h"

#define RELAY_ALL 9
#define RELAY_OP_ON 1
#define RELAY_OP_OFF 0
#define RELAY_OP_TOGGLE -1
#define RELAY_CARD1 0
#define RELAY_CARD2 1
#define RELAY_CARD3 2
#define RELAY_ADDR1 0x72
#define RELAY_ADDR2 0x74
#define RELAY_ADDR3 0x78

#define RELAY_CTRL_MANUAL (1<<0)	//Manual control
#define RELAY_CTRL_SENSOR (1<<1)	//Sensor control
#define RELAY_CTRL_TIMER (1<<2)	//Timer control
#define RELAY_CTRL_STATE (1<<3)	//Control state 1=on, 0=off
#define RELAY_CTRL_SENSOR_ID 0xF0	//High bits contain sensor id

#define RELAY_TIMER_START 1
#define RELAY_TIMER_END 0

#define RELAY_TAG_SIZE 16

void relay_init();

void relay_switch(int card, int relay, int operation);

void relay_update();

unsigned char relay_get_state(int card);
void relay_set_state(int card, unsigned char state);

unsigned char relay_get_ctrl(int card, int relay);
void relay_set_ctrl(int card, int relay, unsigned char ctrl);
void relay_set_ctrl_method(int card, int relay, unsigned char method);

int relay_get_sensor(int card, int relay);
void relay_set_sensor(int card, int relay, int value);

void relay_get_timer(int card, int relay, uint8_t type, uint8_t* hour, uint8_t* min);
void relay_set_timer(int card, int relay, uint8_t type, uint8_t hour, uint8_t min);

int relay_set_tag(uint8_t len, const char* tag, int card, int relay);

int relay_print_tag(char* buf, int card, int relay);

#endif /* RELAY_H */