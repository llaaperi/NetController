/*
 * tcp.c
 *
 * Created: 10.7.2011 18:55:09
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "network.h"
#include "udp.h"
#include "ip.h"


#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lcd.h"
//sprintf_P(lcd_buf_l1, PSTR("Invalid ARP:"));
//sprintf_P(lcd_buf_l1, PSTR("code: %d"), err);
//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);

struct udp_socket _socket;
struct udp_header _udp_header;


/*
 *
 */
void init_socket(struct udp_socket* socket, uint8_t* src_ip_addr, uint16_t src_port){
}


/*
 * Calculate checksum over the whole packet, includind the payload.
 */
uint16_t udp_chc(uint16_t len, const uint8_t* data, const uint8_t* dest_ip){
    return 0x0000;
}


/*
 *
 */
int tcp_parse_header(struct udp_header* header, uint8_t* packet, uint16_t pkt_len){
	return 0;
}


/*
 *
 */
void udp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr){
}


/*
 *
 */
void udp_send(struct udp_socket* socket, uint8_t flags, uint16_t pkt_len){
}
