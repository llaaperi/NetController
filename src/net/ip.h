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
#define IP_H_DST 16

#define IP_HEADER_SIZE 20
#define IP_ADDR_LEN 4


struct ip_header{
	uint8_t version;
	uint8_t ihl;	//Internet Header Length
	uint16_t tos;	//Type of service and congestion notification
	uint16_t len;	//Total length (header + data)
	uint16_t id;	//identification
	uint16_t flfr;	//Flags and fragment offset
	uint8_t ttl;	//Time to live
	uint8_t prot;	//Payload protocol
	uint16_t hchc;	//Header checksum
	uint8_t src_ip[4];
	uint8_t dst_ip[4];
	
	uint8_t* payload;
	uint16_t payload_len;
};

/*
 *	Function calculates checksum of the header
 */
uint16_t ip_chc(uint16_t len, const uint8_t* data);


/*
 *	Receive IP packet
 */
void ip_recv(uint8_t* packet, uint16_t pkt_len);


/*
 *	Send IP packet
 */
void ip_send(const uint8_t* dst_ip, uint8_t protocol, uint16_t pkt_len);

#endif /* IP_H */