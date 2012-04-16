/*
 * icmp.c
 *
 * Created: 8.7.2011 17:17:37
 *  Author: Lauri L‰‰peri
 */ 

#include <stdlib.h>
#include "network.h"
#include "icmp.h"
#include "ip.h"


struct icmp_header _icmp_header;


/*
 *
 */
int icmp_parse_header(struct icmp_header* header, uint8_t* packet, uint16_t pkt_len){
	
	if(pkt_len < ICMP_HEADER_SIZE){
		return -1;
	}
	
	header->type = packet[ICMP_H_TYPE];
	header->code = packet[ICMP_H_CODE];
	
	header->chc = (packet[ICMP_H_CHC] << 8) | (packet[ICMP_H_CHC + 1] & 0xFF);
	
	header->data = packet[ICMP_H_DATA];
	header->data <<= 8;
	header->data |= packet[ICMP_H_DATA + 1] & 0xFF;
	header->data <<= 8;
	header->data |= packet[ICMP_H_DATA + 2] & 0xFF;
	header->data <<= 8;
	header->data |= packet[ICMP_H_DATA + 3] & 0xFF;
	return 0;
}


/*
 *
 */
void icmp_recv(uint8_t* packet, uint16_t pkt_len){
	
	//Parse and check packet validity
	if(icmp_parse_header(&_icmp_header, packet, pkt_len)){
		return;
	}
	
	//Handle echorequest
	if(_icmp_header.type == ICMP_TYPE_ECHOREQUEST && _icmp_header.code == ICMP_CODE_ECHOREQUEST){
		icmp_send(NULL, ICMP_TYPE_ECHOREPLY, ICMP_CODE_ECHOREPLY, _icmp_header.data, pkt_len);
		return;
	}
}


/*
 *
 */
void icmp_send(const uint8_t* dst_ip, uint8_t type, uint8_t code, uint32_t data, uint16_t pkt_len){
	
	uint8_t* packet = _network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	
	packet[ICMP_H_TYPE] = type;
	packet[ICMP_H_CODE] = code;
	packet[ICMP_H_CHC] = 0x00;			//Zero Checksum
	packet[ICMP_H_CHC + 1] = 0x00;
	
	packet[ICMP_H_DATA] = (data >> 24);
	packet[ICMP_H_DATA + 1] = (data >> 16) & 0xFF;
	packet[ICMP_H_DATA + 2] = (data >> 8) & 0xFF;
	packet[ICMP_H_DATA + 3] = (data & 0xFF);
	
	//Calculate checksum
	uint16_t chc = ip_chc(pkt_len, packet);
	packet[ICMP_H_CHC] = (chc >> 8);			//Checksum
	packet[ICMP_H_CHC + 1] = (chc & 0xFF);	
	
	ip_send(dst_ip, IP_PROT_ICMP, pkt_len);
}
