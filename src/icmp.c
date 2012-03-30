/*
 * icmp.c
 *
 * Created: 8.7.2011 17:17:37
 *  Author: Lauri Lääperi
 */ 

#include "icmp.h"

void icmp_recv(uint16_t len, const uint8_t* packet, const uint8_t* src_ip_addr){
	
	if((packet[ICMP_H_TYPE] == ICMP_TYPE_ECHOREQUEST) &&  packet[ICMP_H_CODE] == ICMP_CODE_ECHOREQUEST){	//Echo request
		
		uint8_t *reply = eth_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE;
		
		reply[ICMP_H_TYPE] = ICMP_TYPE_ECHOREPLY;
		reply[ICMP_H_CODE] = ICMP_CODE_ECHOREPLY;
		reply[ICMP_H_CHC] = 0x00;			//Checksum
		reply[ICMP_H_CHC + 1] = 0x00;
		//reply[ICMP_H_ID] = packet[ICMP_H_ID];			//Identifier
		//reply[ICMP_H_ID + 1] = packet[ICMP_H_ID + 1];
		//reply[ICMP_H_SQ] = packet[ICMP_H_SQ];	//Sequence number
		//reply[ICMP_H_SQ + 1] = packet[ICMP_H_SQ + 1];
		
		//memcpy(&reply[8], &packet[8], (len - 8));	//Data
		
		uint16_t chc = ip_chc(len, reply);
		reply[ICMP_H_CHC] = (chc>>8);			//Checksum
		reply[ICMP_H_CHC + 1] = (chc & 0xFF);
		
		ip_send(len, src_ip_addr);
	}
	
}

