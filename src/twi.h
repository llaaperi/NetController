/*
 * twi.h
 *
 * Created: 11.6.2011 1:49:59
 *  Author: Lauri
 */ 


#ifndef TWI_H
#define TWI_H

#include <stdint.h>


/*
 *
 */
void twi_init(uint32_t f_cpu);


/*
 *
 */
void twi_write_byte(uint8_t addr, uint8_t byte);


/*
 *
 */
int twi_read_byte(uint8_t addr, uint8_t* byte);



#endif /* TWI_H */
