/*
 * udp.c
 *
 * Created: 31.1.2014 18:55:09
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "network.h"
#include "udp.h"
#include "ncp.h"
#include "ip.h"


#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lcd.h"
//sprintf_P(lcd_buf_l1, PSTR("Invalid ARP:"));
//sprintf_P(lcd_buf_l1, PSTR("code: %d"), err);
//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);

struct udp_socket _socket;
struct udp_header _header;


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
int parse_header(struct udp_header* header, uint8_t* packet, uint16_t pkt_len){
    
    //Check that atleast header is received
	if(pkt_len < UDP_HEADER_SIZE){
		return -1;
	}
    
    //Ports
	header->src_port = (packet[UDP_H_SRC] << 8) | (packet[UDP_H_SRC + 1] & 0xFF);
	header->dst_port = (packet[UDP_H_DST] << 8) | (packet[UDP_H_DST + 1] & 0xFF);
    header->length = (packet[UDP_H_LEN] << 8) | (packet[UDP_H_LEN + 1] & 0xFF);
    header->checksum = (packet[UDP_H_CHC] << 8) | (packet[UDP_H_CHC + 1] & 0xFF);
    
    header->payload = packet + UDP_HEADER_SIZE;
    header->payload_len = header->length - UDP_HEADER_SIZE;
    
	return 0;
}


void printInfo(uint8_t* src_ip_addr){
    sprintf_P(lcd_buf_l1, PSTR("%u.%u.%u.%u:%u"), src_ip_addr[0], src_ip_addr[1], src_ip_addr[2], src_ip_addr[3], _header.src_port);
    sprintf_P(lcd_buf_l2, PSTR("p:%u l:%d"), _header.dst_port, _header.length);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}


/*
 *
 */
void udp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr){
    
    //Parse header
    parse_header(&_header, packet, pkt_len);
    
    //printInfo(src_ip_addr);
    if(_header.dst_port == UDP_PORT_NCP){
        ncp_recv(&_socket, _header.payload, _header.payload_len);
    }
}


/*
 *
 */
void udp_send(struct udp_socket* socket, uint8_t flags, uint16_t pkt_len){
}
