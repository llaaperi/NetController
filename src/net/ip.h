/*
 * ip.h
 *
 * Created: 8.7.2011 16:18:46
 *  Author: Lauri
 */ 


#ifndef IP_H
#define IP_H

#include "eth.h"
#include "icmp.h"
#include "tcp.h"

#define IP_VERSION 0x40
#define IP_IHL 0x05
#define IP_TOS 0x00
#define IP_FL 0x00
#define IP_FR 0x00
#define IP_TTL 64

//Protocol numbers
#define IP_PROT_ICMP 1
#define IP_PROT_TCP 6
#define IP_PROT_UDP 17

//Header offsets
#define IP_H_VESRION 0
#define IP_H_IHL 0
#define IP_H_TOS 1
#define IP_H_LEN 2
#define IP_H_ID 4
#define IP_H_FL 6
#define IP_H_FR 6
#define IP_H_TTL 8
#define IP_H_PROT 9
#define IP_H_CHC 10
#define IP_H_SRC 12
#define IP_H_DEST 16

#define IP_HEADER_SIZE 20
#define IP_ADDR_LEN 4


/*
 *	Function calculates checksum of the header
 */
uint16_t ip_chc(uint16_t len, const uint8_t* data);


/*
 *	Receive IP packet
 */
uint16_t ip_recv(uint8_t* packet, uint16_t pkt_len);


/*
 *	Send IP packet
 */
void ip_send(uint16_t len, const uint8_t* dest_ip_addr);

#endif /* IP_H */