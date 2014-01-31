/*
 * udp.h
 *
 * Created: 31.1.2014 18:54:49
 *  Author: Lauri L‰‰peri
 */ 


#ifndef UDP_H
#define UDP_H

#define UDP_H_SRC 0
#define UDP_H_DST 2
#define UDP_H_SEQ 4
#define UDP_H_ACK 8
#define UDP_H_OFFSET 12
#define UDP_H_FLAGS_H 12
#define UDP_H_FLAGS 13
#define UDP_H_WSIZE 14
#define UDP_H_CHC 16
#define UDP_H_URG 18
#define UDP_H_OPT 20

#define UDP_HEADER_SIZE 20

#define UDP_OFFSET 0x50
#define UDP_FLAG_FIN 0x01
#define UDP_FLAG_SYN 0x02
#define UDP_FLAG_RST 0x04
#define UDP_FLAG_PSH 0x08
#define UDP_FLAG_ACK 0x10
#define UDP_FLAG_URG 0x20
#define UDP_FLAG_ECE 0x40
#define UDP_FLAG_CWR 0x80
#define UDP_FLAG_NS 0x01

#define UDP_PORT 9876

#include <stdint.h>
#include "../time.h"

struct udp_header{
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
	
	uint8_t* payload;
	uint16_t payload_len;
};


struct udp_socket{
	uint16_t dest_port;
	uint8_t dest_ip[4];
};


/*
 *
 */
void udp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr);


/*
 *
 */
void udp_send(struct udp_socket* socket, uint8_t type, uint16_t pkt_len);


#endif /* UDP_H */
