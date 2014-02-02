/*
 * ip.c
 *
 * Created: 8.7.2011 16:42:31
 *  Author: Lauri
 */ 

#include <string.h>
#include "network.h"
#include "eth.h"
#include "ip.h"

//For debugging LCD
//#include <stdio.h>
//#include <avr/pgmspace.h>
//#include "../lcd.h"

struct ip_header _ip_header;	//Most recent IP header


/*
 *
 */
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


/*
 *
 */
int ip_parse_header(struct ip_header* header, uint8_t* packet, uint16_t pkt_len){
	
	//Check that atleast IP header is received
	if(pkt_len < IP_HEADER_SIZE){
		return -1;
	}
	
	header->version = ((packet[IP_H_VESRION] & 0xF0) >> 4);
	if(header->version != (IP_VERSION >> 4)){
		return -1;
	}
	
	header->ihl = packet[IP_H_IHL] & 0x0F;	//IP header length in bytes
	header->len = (packet[IP_H_LEN] << 8) | packet[IP_H_LEN + 1];
	
	memcpy(header->src_ip, &packet[IP_H_SRC], 4);
	memcpy(header->dst_ip, &packet[IP_H_DST], 4);
	
	header->payload = packet + (header->ihl * 4);
	header->payload_len = header->len - (header->ihl * 4);	//Calculate payload length from IP header data
	if(header->payload_len > (pkt_len - (header->ihl * 4))){	//Check that payload length is not limited by buffer length
		header->payload_len = (pkt_len - (header->ihl * 4));
	}
	
	return 0;
}


/*
 *	Receive IP packets.
 *	Return length of the reply packet which is to be sent after the function returns
 */
void ip_recv(uint8_t* packet, uint16_t pkt_len){
	
	//Parse and check header
	if(ip_parse_header(&_ip_header, packet, pkt_len)){
		return;
	}
	
	//Stop if there is no payload
	if(_ip_header.payload_len == 0){
		return;
	}

	//Handle ICMP packets
	if(packet[IP_H_PROT] == IP_PROT_ICMP){
		icmp_recv(_ip_header.payload, _ip_header.payload_len);
		return;
	}
	
    //DISABLED, MUST BE UNCOMMENTED FROM IP.H
	//Handle TCP packets
	if(packet[IP_H_PROT] == IP_PROT_TCP){
		//sprintf(lcd_buf_l1, "TCP");
		//sprintf(lcd_buf_l2, "len %u", payload_len);
		//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		//tcp_recv(_ip_header.payload, _ip_header.payload_len, _ip_header.src_ip);
		return;
	}
	
    int reply_len = 0;
	//Handle UDP packets
	if(packet[IP_H_PROT] == IP_PROT_UDP){
        reply_len = udp_recv(_ip_header.payload, _ip_header.payload_len, _ip_header.src_ip);
        if(reply_len >= 0){
            ip_send(_ip_header.src_ip, IP_PROT_UDP, reply_len); //Reply to source
        }
	}
}


/*
 * dest_ip_addr	Destination IP address, if NULL -> reply to src
 * protocol		Upper layer protocol type
 * pkt_len		Length of the IP packet payload
 */
void ip_send(const uint8_t* dst_ip, uint8_t protocol, uint16_t pkt_len){
	
	uint8_t* packet = _network_buf + ETH_HEADER_SIZE;	//Write directly to network buffer
	
	//Copy dest ip from the recent received packet id destination is undefined
	if(dst_ip == NULL){
		dst_ip = _ip_header.src_ip;
	}
	
	packet[IP_H_VESRION] = IP_VERSION;		//Version
	packet[IP_H_IHL] |= IP_IHL;				//Header length
	packet[IP_H_TOS] = 0x00;				//Type of Service
	
	packet[IP_H_LEN] = ((pkt_len + IP_HEADER_SIZE)>>8);	//Total packet length
	packet[IP_H_LEN + 1] = ((pkt_len + IP_HEADER_SIZE) & 0xFF);
	
	packet[IP_H_ID] = 0x00;						//Identification
	packet[IP_H_ID + 1] = 0x01;
	
	packet[IP_H_FL] = (0x00 & 0xE0);			//Flags
	packet[IP_H_FR] |= (0x00 & 0x1F);			//Fragment offset
	packet[IP_H_FR + 1] = 0x00;
	packet[IP_H_TTL] = IP_TTL;					//Time to Live
	
	packet[IP_H_PROT] = protocol;		//Upper layer protocol
	
	packet[IP_H_CHC] = 0x00;			//Zero header checksum
	packet[IP_H_CHC + 1] = 0x00;
	
	memcpy(&packet[IP_H_SRC], _network_ip_addr, 4);	//Sender IP
	memcpy(&packet[IP_H_DST], dst_ip, 4);	//Destination IP
	
	uint16_t checksum = ip_chc(IP_HEADER_SIZE, packet);
	packet[IP_H_CHC] = (checksum>>8);						//Add valid checksum
	packet[IP_H_CHC + 1] = (checksum & 0xFF);
	
	//Find mac
	//uint8_t* dst_mac = arp_resolve(dst_ip);
	
	eth_send(NULL, ETH_TYPE_IP, pkt_len + IP_HEADER_SIZE);
}
