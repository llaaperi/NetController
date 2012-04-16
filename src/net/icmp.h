/*
 * icmp.h
 *
 * Created: 8.7.2011 17:17:22
 *  Author: Lauri L‰‰peri
 */ 


#ifndef ICMP_H
#define ICMP_H

#define ICMP_H_TYPE 0
#define ICMP_H_CODE 1
#define ICMP_H_CHC 2
#define ICMP_H_DATA 4
#define ICMP_H_ID 4	//Echo id
#define ICMP_H_SQ 6	//Echo sequence number

#define ICMP_HEADER_SIZE 8

#define ICMP_TYPE_ECHOREPLY 0
#define ICMP_CODE_ECHOREPLY 0

#define ICMP_TYPE_ECHOREQUEST 8
#define ICMP_CODE_ECHOREQUEST 0

#include <stdint.h>

struct icmp_header{
	uint8_t type;	//ICMP type
	uint8_t code;	//ICMP Code
	uint16_t chc;	//Checksum
	uint32_t data;	//Type and code dependent data
};


/*
 *
 */
void icmp_recv(uint8_t* packet, uint16_t pkt_len);


/*
 *
 */
void icmp_send(const uint8_t* dst_ip, uint8_t type, uint8_t code, uint32_t data, uint16_t pkt_len);

#endif /* ICMP_H */