/*
 * network.c
 *
 * Created: 7.7.2011 8:20:16
 *  Author: Lauri
 */ 

#ifdef TEST
	#include <string.h>
	#include "../../tests/mockEnc28j60.h"
	#define eeprom_read_block memcpy
	#define eeprom_write_block(X, Y, Z) memcpy(Y, X, Z)
	#define EEMEM 
#else
	#include <avr/eeprom.h>
	#include "enc28j60.h"
#endif

#include "network.h"
#include "eth.h"

//Network addresses on EEPROM
uint8_t EEMEM _eeprom_mac_addr[6] = {0x00, 0xFA, 0x20, 0xFA, 0x47, 0x31};
uint8_t EEMEM _eeprom_ip_addr[4] = {10, 0, 0, 5};

//Runtime network addresses (used within net)
uint8_t _network_mac_addr[6] = {0,0,0,0,0,0};
uint8_t _network_ip_addr[4] = {0,0,0,0};

volatile char _network_flag = 0;	//currently not used

/*
 *
 */
void network_init(){
	
	//Read network addresses from the EEPROM
	eeprom_read_block(_network_mac_addr, _eeprom_mac_addr, 6);
	eeprom_read_block(_network_ip_addr, _eeprom_ip_addr, 4);
	
	//Initialize ENC28J60
	enc28j60_init(_network_mac_addr);	
}

/*
 *	Receive packet
 */
void network_recv(){
	
	uint16_t pkt_len = 0;
	pkt_len = enc28j60_recv(NETWORK_BUF_SIZE, _network_buf);
	
	if(pkt_len == 0){	//Return if there is no packet
		return;
	}
	
	eth_recv(_network_buf, pkt_len);
}


/*
 *
 */
void network_send(uint16_t pkt_len){
	
	if(pkt_len > NETWORK_BUF_SIZE){
		pkt_len = NETWORK_BUF_SIZE;
	}
	
	if(pkt_len > 0){
		enc28j60_send(pkt_len, _network_buf);
	}
}


/*
 *
 */
void network_set_mac_addr(const uint8_t* mac_addr){
	eeprom_write_block(mac_addr, _eeprom_mac_addr, 6);	//Write new address to the EEPROM
	network_init();	//Put new address to use
}


/*
 *
 */
const uint8_t* network_get_mac_addr(){
	eeprom_read_block(_network_mac_addr, _eeprom_mac_addr, 6);	//Read address from the EEPROM
	return _network_mac_addr;	//Return pointer to the address
}


/*
 *
 */
void network_set_ip_addr(const uint8_t* ip_addr){
	eeprom_write_block(ip_addr, _eeprom_ip_addr, 4);	//Write new address to the EEPROM
	network_init();	//Put new address to use
}


/*
 *
 */
const uint8_t* network_get_ip_addr(){
	eeprom_read_block(_network_ip_addr, _eeprom_ip_addr, 4);	//Read address from the EEPROM
	return _network_ip_addr;	//Return pointer
}


/*
 *
 */
ISR(INT1_vect){
	_network_flag = 1;
}
