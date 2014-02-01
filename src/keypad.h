/*
 * keypad.h
 *
 * Created: 10.6.2011 23:11:58
 *  Author: Lauri
 */ 


#ifndef KEYPAD_H
#define KEYPAD_H

#define F_CPU 12500000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"

#define KEYPAD_ADDR 0x70

void keypad_init();

char keypad_get();

/*
 * This is blocking version of keypad_get()
 */
char keypad_get_input();

ISR(INT0_vect);

#endif /* KEYPAD_H_ */
