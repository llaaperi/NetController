/*
 * eth.c
 *
 * Created: 7.7.2011 8:20:16
 *  Author: Lauri
 */ 

#include <string.h>
#include "network.h"
#include "eth.h"
#include "arp.h"
#include "ip.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lcd.h"

struct eth_header _eth_header;	//Most recently received header

/*
 * Parse header.
 * Return 0 if valid header, -1 if invalid
 */
int eth_parse_header(struct eth_header* header, uint8_t* packet, uint16_t pkt_len){
	
	//Check packet length
	if(pkt_len < ETH_HEADER_SIZE){
		return -1;
	}
	
	memcpy(header->dst_mac, &packet[ETH_H_MAC_DST], 6);	//Copy dst address
	memcpy(header->src_mac, &packet[ETH_H_MAC_SRC], 6);	//Copy src address
	
	header->type = (packet[ETH_H_TYPE_H] << 8) | (packet[ETH_H_TYPE_L] & 0xFF);	//Payload type
	
	//Check payload type (accept only ARP and IP)
	if(header->type != ETH_TYPE_ARP && header->type != ETH_TYPE_IP){
		return -1;
	}
	
	header->payload = &packet[ETH_HEADER_SIZE];
	header->payload_len = pkt_len - ETH_HEADER_SIZE;
	
	return 0;
}


/*
 *
 */
void eth_recv(uint8_t* packet, uint16_t pkt_len){
	
	//Parse header and stop if it's invalid
	if(eth_parse_header(&_eth_header, packet, pkt_len)){
		return;
	}
	
	//Handle ARP packets
	if(_eth_header.type == ETH_TYPE_ARP){
		arp_recv(_eth_header.payload, _eth_header.payload_len);
		return;
	}
	
	//Handle IP packets
	if(_eth_header.type == ETH_TYPE_IP){
		ip_recv(_eth_header.payload, _eth_header.payload_len);
		return;
	}
}


/*
 *
 */
void eth_send(const uint8_t* dst_mac, uint16_t type, uint16_t pkt_len){
	
	//Use recent source is destination is no specified
	if(dst_mac == NULL){
		dst_mac = _eth_header.src_mac;
	}
	
	//sprintf_P(lcd_buf_l1, PSTR("ETH: Send"));
	//sprintf_P(lcd_buf_l2, PSTR("%02x%02x%02x%02x%02x%02x"), dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
	//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
	//Add Ethernet header and send
	memcpy(&_network_buf[ETH_H_MAC_DST], dst_mac, 6);	//Destination MAC
	memcpy(&_network_buf[ETH_H_MAC_SRC], _network_mac_addr, 6);	//My MAC
	_network_buf[ETH_H_TYPE_H] = (type >> 8);	//Type high bits
	_network_buf[ETH_H_TYPE_L] = (type & 0xFF);	//Type low bits
	
	network_send(pkt_len + ETH_HEADER_SIZE);
}