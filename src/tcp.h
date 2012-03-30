/*
 * tcp.h
 *
 * Created: 10.7.2011 18:54:49
 *  Author: Lauri L‰‰peri
 */ 


#ifndef TCP_H
#define TCP_H

#include "ip.h"
#include "http.h"
#include "lcd.h"

#define TCP_H_SRC 0
#define TCP_H_DEST 2
#define TCP_H_SQ 4
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

typedef struct{
	uint8_t active;
	uint32_t seq;
	uint32_t ack;
	uint16_t dest_port;
	uint8_t dest_ip[4];
} Tcp_socket;

void tcp_recv(uint16_t len, const uint8_t* packet, const uint8_t* src_ip_addr);
void tcp_send(uint16_t len, uint8_t type);

#endif /* TCP_H */
