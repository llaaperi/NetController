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
#define UDP_H_LEN 4
#define UDP_H_CHC 6

#define UDP_HEADER_SIZE 8

#define UDP_PORT_NCP 9876

#include <stdint.h>

struct udp_header{
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
	
	uint8_t* payload;
	uint16_t payload_len;
};


struct udp_socket{
	uint16_t dst_port;
	uint8_t dst_ip[4];
};


/*
 *
 */
int udp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr);


/*
 *
 */
void udp_send(struct udp_socket* socket, uint16_t pkt_len);


#endif /* UDP_H */
