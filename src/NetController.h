/*
 * NetController.h
 *
 * Created: 11.6.2011 2:05:10
 *  Author: Lauri
 */ 


#ifndef NETCONTROLLER_H
#define NETCONTROLLER_H


//#define F_CPU 6250000UL
//#define F_CPU 8000000UL
#define F_CPU 12500000UL

#define ERROR 0x1
#define OK 0x2

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>


#include "keypad.h"
#include "lcd.h"
#include "relay.h"
#include "ds1820.h"
#include "enc28j60.h"
#include "eth.h"
#include "arp.h"

void state(char led);

void timer_ms_start();
void timer_ms_stop();
void timer_ms_print();

int clock_print(char *buf);
int time_print(char *buf);

void netcontroller_get_time(uint8_t* hour, uint8_t* min);


#endif /* NETCONTROLLER_H */