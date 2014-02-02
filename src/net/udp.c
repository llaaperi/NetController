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


/**
 *
 */
void printInfo(uint8_t* src_ip_addr){
    sprintf_P(lcd_buf_l1, PSTR("%u.%u.%u.%u:%u"), src_ip_addr[0], src_ip_addr[1], src_ip_addr[2], src_ip_addr[3], _header.src_port);
    sprintf_P(lcd_buf_l2, PSTR("p:%u l:%d"), _header.dst_port, _header.length);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}


/**
 *
 */
void init_socket(struct udp_socket* socket, uint8_t* src_ip_addr, uint16_t src_port){
    memcpy(&socket->dst_port, &src_port, 2);
    memcpy(&socket->dst_ip, src_ip_addr, 4);
}


/**
 * Calculate checksum
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
		return 0;
	}
    
    //Ports
	header->src_port = (packet[UDP_H_SRC] << 8) | (packet[UDP_H_SRC + 1] & 0xFF);
	header->dst_port = (packet[UDP_H_DST] << 8) | (packet[UDP_H_DST + 1] & 0xFF);
    //Length
    header->length = (packet[UDP_H_LEN] << 8) | (packet[UDP_H_LEN + 1] & 0xFF);
    //Checksum
    header->checksum = (packet[UDP_H_CHC] << 8) | (packet[UDP_H_CHC + 1] & 0xFF);
    //Payload
    header->payload = packet + UDP_HEADER_SIZE;
    header->payload_len = header->length - UDP_HEADER_SIZE;
    
    return 1;
}


static inline void write_header(struct udp_socket* socket, uint16_t pkt_len){
    uint8_t* packet = _network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE;
    
    packet[UDP_H_SRC] = (UDP_PORT_NCP >> 8);		//Source port
	packet[UDP_H_SRC + 1] = (UDP_PORT_NCP & 0xFF);
	
	packet[UDP_H_DST] = (socket->dst_port >> 8);	//Destination port
	packet[UDP_H_DST + 1] = (socket->dst_port & 0xFF);
    
    packet[UDP_H_LEN] = ((pkt_len + UDP_HEADER_SIZE) >> 8);   //Packet length (header + payload)
    packet[UDP_H_LEN + 1] = ((pkt_len + UDP_HEADER_SIZE) & 0xFF);
    
    packet[UDP_H_CHC] = 0x00;
    packet[UDP_H_CHC + 1] = 0x00;
}

/*
 *
 */
int udp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr){
    
    //Parse header
    if(!parse_header(&_header, packet, pkt_len)){
        return -1;
    }
    
    init_socket(&_socket, src_ip_addr, _header.src_port);
    
    sprintf_P(lcd_buf_l1, PSTR("src:%u"), _socket.dst_port);
    //sprintf_P(lcd_buf_l2, PSTR("dst:%u"), _socket.dst_port);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
    
    int reply_len = 0;
    
    //printInfo(src_ip_addr);
    if(_header.dst_port == UDP_PORT_NCP){
        reply_len = ncp_recv(_header.payload, _header.payload_len);
    }
    
    if(reply_len >= 0){   //Reply (allow zero length payload)
        write_header(&_socket, (uint16_t)reply_len);
    }
    
    return UDP_HEADER_SIZE + reply_len;
}


/*
 *
 */
void udp_send(struct udp_socket* socket, uint16_t pkt_len){
    
    write_header(socket, pkt_len);
    
    //sprintf_P(lcd_buf_l1, PSTR("src:%u"), socket->dst_port);
    //sprintf_P(lcd_buf_l2, PSTR("dst:%u"), socket->dst_port);
    //lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
    
    ip_send(socket->dst_ip, IP_PROT_UDP, pkt_len + UDP_HEADER_SIZE);
}
