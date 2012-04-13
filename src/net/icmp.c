/*
 * icmp.c
 *
 * Created: 8.7.2011 17:17:37
 *  Author: Lauri Lääperi
 */ 

#include "icmp.h"
#include "ip.h"

uint16_t icmp_recv(uint8_t* packet, uint16_t pkt_len){
	
	if((packet[ICMP_H_TYPE] == ICMP_TYPE_ECHOREQUEST) &&  packet[ICMP_H_CODE] == ICMP_CODE_ECHOREQUEST){	//Echo request
		
		packet[ICMP_H_TYPE] = ICMP_TYPE_ECHOREPLY;
		packet[ICMP_H_CODE] = ICMP_CODE_ECHOREPLY;
		packet[ICMP_H_CHC] = 0x00;			//Checksum
		packet[ICMP_H_CHC + 1] = 0x00;
		//reply[ICMP_H_ID] = packet[ICMP_H_ID];			//Identifier
		//reply[ICMP_H_ID + 1] = packet[ICMP_H_ID + 1];
		//reply[ICMP_H_SQ] = packet[ICMP_H_SQ];	//Sequence number
		//reply[ICMP_H_SQ + 1] = packet[ICMP_H_SQ + 1];
		
		//memcpy(&reply[8], &packet[8], (len - 8));	//Data
		
		uint16_t chc = ip_chc(pkt_len, packet);
		packet[ICMP_H_CHC] = (chc>>8);			//Checksum
		packet[ICMP_H_CHC + 1] = (chc & 0xFF);
		
		return pkt_len;	//Return whole packet
	}
	return 0;
}

