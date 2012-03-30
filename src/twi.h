/*
 * twi.h
 *
 * Created: 11.6.2011 1:49:59
 *  Author: Lauri
 */ 


#ifndef TWI_H
#define TWI_H

#include <util/twi.h>

void twi_init();

void twi_write_byte(char addr, char byte);
char twi_read_byte(char addr, char* byte);



#endif /* TWI_H */
