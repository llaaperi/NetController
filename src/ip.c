/*
 * ip.c
 *
 * Created: 8.7.2011 16:42:31
 *  Author: Lauri
 */ 

#include "ip.h"

//static uint8_t dest_ip_addr[4];

uint16_t ip_chc(uint16_t len, const uint8_t* data){
	
	uint32_t sum = 0;
	uint16_t chc = 0;
	
	int i = 0;
	for(; i < (len - 1); i += 2){
		sum += (0xFFFF & (((data[i] & 0xFF)<<8) | (data[i + 1] & 0xFF)));
	}
	
	if(i < len){
		sum += ((data[i] & 0xFF)<<8);
	}
	
	while(sum>>16){
		sum = (sum & 0xFFFF) + (sum>>16);
	}
	
	chc = 0xFFFF ^ sum;
	
	return chc;
}



void ip_recv(const uint8_t* packet){
	
	if((packet[IP_H_VESRION] & 0xF0) != IP_VERSION){	//Not IPv4
		return;
	}
	
	uint16_t pkt_len = (packet[IP_H_LEN]<<8) | packet[IP_H_LEN + 1];
	uint16_t payload_idx = (packet[IP_H_IHL] & 0x0F) * 4;		//Payload offset (in case header options are used)
	uint16_t payload_len = pkt_len - payload_idx;
	
	//memcpy(dest_ip_addr, &packet[IP_H_SRC], 4);
	
	if(payload_len != 0){
		
		if(packet[IP_H_PROT] == IP_PROT_ICMP){		//ICMP packet
			icmp_recv(payload_len, &packet[payload_idx], &packet[IP_H_SRC]);
		}
	
		if(packet[IP_H_PROT] == IP_PROT_UDP){		//UDP packet
			return;
		}
	
		if(packet[IP_H_PROT] == IP_PROT_TCP){		//TCP packet
			/*
			sprintf(lcd_buf_l1, "TCP");
			sprintf(lcd_buf_l2, "len %u", payload_len);
			lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
			*/
			tcp_recv(payload_len, &packet[payload_idx], &packet[IP_H_SRC]);
		}
		
	}
	
}

void ip_send(uint16_t len, const uint8_t* dest_ip_addr){
	
	uint8_t* packet = (eth_buf + ETH_HEADER_SIZE);
	uint16_t checksum = 0;
	
	packet[IP_H_VESRION] = IP_VERSION;		//Version
	packet[IP_H_IHL] |= IP_IHL;				//Header length
	packet[IP_H_TOS] = 0x00;				//Type of Service
	
	packet[IP_H_LEN] = ((len + IP_HEADER_SIZE)>>8);	//Total packet length
	packet[IP_H_LEN + 1] = ((len + IP_HEADER_SIZE) & 0xFF);
	
	packet[IP_H_ID] = 0x00;						//Identification
	packet[IP_H_ID + 1] = 0x01;
	
	packet[IP_H_FL] = (0x00 & 0xE0);			//Flags
	packet[IP_H_FR] |= (0x00 & 0x1F);			//Fragment offset
	packet[IP_H_FR + 1] = 0x00;
	packet[IP_H_TTL] = IP_TTL;					//Time to Live
	
	//packet[IP_H_PROT] = eth_buf[ETH_HEADER_SIZE + IP_H_PROT];		//Upper layer protocol
	
	packet[IP_H_CHC] = 0x00;			//Zero header checksum
	packet[IP_H_CHC + 1] = 0x00;
	
	memcpy(&packet[IP_H_DEST], dest_ip_addr, 4);	//Destination IP
	memcpy(&packet[IP_H_SRC], eth_ip_addr, 4);	//Sender IP
	
	checksum = ip_chc(IP_HEADER_SIZE, packet);
	packet[IP_H_CHC] = (checksum>>8);						//Add valid checksum
	packet[IP_H_CHC + 1] = (checksum & 0xFF);
	
	eth_send(len + IP_HEADER_SIZE);
}
