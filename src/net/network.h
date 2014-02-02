/*
 * network.h
 *
 * Created: 7.7.2011 8:19:33
 *  Author: Lauri
 */ 


#ifndef NETWORK_H
#define NETWORK_H

#define NETWORK_BUF_SIZE 800

//Compiler modifications for tests
#ifdef TEST	//Compiling for test
	#define ISR void network_interrupt_handler
	#define INT1_vect void
#else	//Compiling for AVR
	#include <avr/interrupt.h>
#endif

#include <stdint.h>

//Network addresses
//extern uint8_t EEMEM _eeprom_mac_addr[6];
//extern uint8_t EEMEM _eeprom_ip_addr[4];
extern uint8_t _network_mac_addr[6];
extern uint8_t _network_ip_addr[4];

uint8_t _network_buf[NETWORK_BUF_SIZE];

/*
 *
 */
void network_init();


/*
 *
 */
void network_recv();


/*
 *
 */
void network_send(uint16_t pkt_len);


/*
 *	Set and get mac address to/from the EEPROM
 */
void network_set_mac_addr(const uint8_t* mac_addr);
const uint8_t* network_get_mac_addr();


/*
 *	Set and get IP address to/from the EEPROM
 */
void network_set_ip_addr(const uint8_t* ip_addr);
const uint8_t* network_get_ip_addr();


/*
 *	Interrupt handler for new packets
 */
ISR(INT1_vect);

#endif /* NETWORK_H_ */