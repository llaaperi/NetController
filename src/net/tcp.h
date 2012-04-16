/*
 * tcp.h
 *
 * Created: 10.7.2011 18:54:49
 *  Author: Lauri L‰‰peri
 */ 


#ifndef TCP_H
#define TCP_H

#define TCP_H_SRC 0
#define TCP_H_DST 2
#define TCP_H_SEQ 4
#define TCP_H_ACK 8
#define TCP_H_OFFSET 12
#define TCP_H_FLAGS_H 12
#define TCP_H_FLAGS 13
#define TCP_H_WSIZE 14
#define TCP_H_CHC 16
#define TCP_H_URG 18
#define TCP_H_OPT 20

#define TCP_HEADER_SIZE 20

#define TCP_OFFSET 0x50
#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20
#define TCP_FLAG_ECE 0x40
#define TCP_FLAG_CWR 0x80
#define TCP_FLAG_NS 0x01

#define TCP_MAX_SEG_SIZE 1408
#define TCP_WINDOW_SIZE 1408

#define TCP_MAX_CONN 3

#define TCP_PORT_HTTP 80

#include <stdint.h>

struct tcp_header{
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seqnum;
	uint32_t acknum;
	uint8_t offset;
	uint8_t flags;
	uint16_t window;
	uint16_t chc;
	uint16_t urg;
	
	uint8_t* payload;
	uint16_t payload_len;
};


struct tcp_socket{
	uint8_t active;
	uint8_t handshake;
	uint32_t seq;
	uint32_t ack;
	uint16_t dest_port;
	uint8_t dest_ip[4];
};


/*
 *
 */
void tcp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr);


/*
 *
 */
void tcp_send(struct tcp_socket* socket, uint8_t type, uint16_t pkt_len);

#endif /* TCP_H */
