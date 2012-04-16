/*
 * arp.c
 *
 * Created: 7.7.2011 12:20:46
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "eth.h"
#include "arp.h"
#include "network.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lcd.h"


/*
 * Check ARP packet validity.
 * Return 0 if packet is valid, positive integer if not
 */
int arp_packet_check(uint8_t* packet, uint16_t pkt_len){
	
	if(pkt_len < ARP_PACKET_SIZE){	//Check that full packet is received
		return 1;
	}
	
	if((packet[ARP_P_HTYPE_H] != ARP_HTYPE_H) || (packet[ARP_P_HTYPE_L] != ARP_HTYPE_L)){	//Wrong hardware type
		return 2;
	}
	
	if((packet[ARP_P_PTYPE_H] != ARP_PTYPE_H) || (packet[ARP_P_PTYPE_L] != ARP_PTYPE_L)){	//Wrong protocol type
		return 3;
	}
	
	if(packet[ARP_P_HLEN] != ARP_HLEN){	//Wrong hardware address length
		return 4;
	}
	
	if(packet[ARP_P_PLEN] != ARP_PLEN){	//Wrong protocol address length
		return 5;
	}
	
	if(memcmp(_network_ip_addr, &packet[ARP_P_DST_IP], ARP_PLEN)){	//Wrong destination IP
		return 6;
	}
	return 0;
}


/*
 *
 */
void arp_recv(uint8_t* packet, uint16_t pkt_len){
	
	uint16_t arp_oper = 0;
	
	int err = 0;
	if((err = arp_packet_check(packet, pkt_len))){	//Check packet validity
		//sprintf_P(lcd_buf_l1, PSTR("Invalid ARP:"));
		//sprintf_P(lcd_buf_l2, PSTR("code: %d"), err);
		//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		return;
	}
	
	//Parse operation
	arp_oper = (packet[ARP_P_OPER_H] << 8) | (packet[ARP_P_OPER_L] & 0xFF);
	
	//Handle reply
	if(arp_oper == ARP_OPER_REPLY){
		return;
	}	
	
	//Handle request
	if(arp_oper == ARP_OPER_REQUEST){
		
		//sprintf_P(lcd_buf_l1, PSTR("ARP: Request"));
		//sprintf_P(lcd_buf_l2, PSTR("SRC: %u.%u.%u.%u"),  packet[ARP_P_SRC_IP], packet[ARP_P_SRC_IP+1], packet[ARP_P_SRC_IP+2], packet[ARP_P_SRC_IP+3]);
		//sprintf_P(lcd_buf_l2, PSTR("SRC: %02x%02x%02x%02x"),  packet[ARP_P_SRC_MAC], packet[ARP_P_SRC_MAC+1], packet[ARP_P_SRC_MAC+2], packet[ARP_P_SRC_MAC+3]);
		lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		
		arp_send(ARP_OPER_REPLY, &packet[ARP_P_SRC_MAC], &packet[ARP_P_SRC_IP]);
		return;
	}	
}


/*
 *
 */
void arp_send(uint16_t oper, const uint8_t* dest_mac_addr, const uint8_t* dest_ip_addr){
	
	uint8_t* packet = _network_buf + ETH_HEADER_SIZE;
	
	packet[0] = ARP_HTYPE_H;	//Hardware type (Ethernet)
	packet[1] = ARP_HTYPE_L;
	packet[2] = ARP_PTYPE_H;	//Protocol type	(IP)
	packet[3] = ARP_PTYPE_L;
	packet[4] = ARP_HLEN;		//Hardware address length
	packet[5] = ARP_PLEN;		//Protocol address length
	packet[6] = (oper >> 8);	//Operation, 1 = request, 2 = reply
	packet[7] = (oper & 0xFF);

	memcpy(&packet[ARP_P_DST_IP], dest_ip_addr, ARP_HLEN);			//Receiver's IP address
	
	if(oper == ARP_OPER_REPLY){
		memcpy(&packet[ARP_P_DST_MAC], dest_mac_addr, ARP_HLEN);	//Receiver's MAC address
	}else		
	if(oper == ARP_OPER_REQUEST){
		memset(&packet[ARP_P_DST_MAC], 0xFF, ARP_HLEN);	//Broadcast
	}
	
	memcpy(&packet[ARP_P_SRC_MAC], _network_mac_addr, ARP_HLEN);	//My MAC address
	memcpy(&packet[ARP_P_SRC_IP], _network_ip_addr, ARP_PLEN);		//My IP address
	
	eth_send(NULL, ETH_TYPE_ARP, ARP_PACKET_SIZE);		//Send packet
}
