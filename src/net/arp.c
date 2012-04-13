/*
 * arp.c
 *
 * Created: 7.7.2011 12:20:46
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "arp.h"
#include "network.h"


/*
 * Check ARP packet validity.
 * Return 0 if packet is valid, -1 if not
 */
int arp_packet_check(uint8_t* packet, uint16_t pkt_len){
	
	if(pkt_len < ARP_PACKET_SIZE){	//Check that full packet is received
		return -1;
	}
	
	if((packet[ARP_P_HTYPE_H] != ARP_HTYPE_H) || (packet[ARP_P_HTYPE_L] != ARP_HTYPE_L)){	//Wrong hardware type
		return -1;
	}
	
	if((packet[ARP_P_PTYPE_H] != ARP_PTYPE_H) || (packet[ARP_P_PTYPE_L] != ARP_PTYPE_L)){	//Wrong protocol type
		return -1;
	}
	
	if(packet[ARP_P_HLEN] != ARP_HLEN){	//Wrong hardware address length
		return -1;
	}
	
	if(packet[ARP_P_PLEN] != ARP_PLEN){	//Wrong protocol address length
		return -1;
	}
	
	if(memcmp(_network_ip_addr, &packet[ARP_P_DEST_IP], ARP_PLEN)){	//Wrong destination IP
		return -1;
	}
	return 0;
}


/*
 *
 */
uint16_t arp_recv(uint8_t* packet, uint16_t pkt_len){
	
	uint16_t arp_oper = 0;
	
	if(!arp_packet_check(packet, pkt_len)){	//Check packet validity
		return 0;
	}
	
	arp_oper = (packet[ARP_P_OPER_H]<<8) | (packet[ARP_P_OPER_L] & 0xFF);
	
	if(arp_oper == ARP_OPER_REPLY){		//ARP reply
		return 0;
	}	
	
	if(arp_oper == ARP_OPER_REQUEST){	//ARP request
		
		//First 7 bytes are the same for the response
		packet[ARP_P_OPER_L] = (ARP_OPER_REPLY & 0xFF);	//Change operation to reply
		memcpy(&packet[ARP_P_DEST_MAC], &packet[ARP_P_SRC_MAC], ARP_HLEN);	//Receiver's MAC address
		memcpy(&packet[ARP_P_DEST_IP], &packet[ARP_P_SRC_IP], ARP_PLEN);	//Receiver's IP address
		memcpy(&packet[ARP_P_SRC_MAC], _network_mac_addr, ARP_HLEN);	//My MAC address
		memcpy(&packet[ARP_P_SRC_IP], _network_ip_addr, ARP_PLEN);		//My IP address
		
		return ARP_PACKET_SIZE;	//Return reply length
	}	
	return 0;
}

/*
void arp_send(uint16_t oper, const uint8_t* dest_mac_addr, const uint8_t* dest_ip_addr){
	
		uint8_t* packet = eth_send_buf + ETH_HEADER_SIZE;
		
		packet[0] = ARP_HTYPE_H;		//Hardware type (Ethernet)
		packet[1] = ARP_HTYPE_L;
		packet[2] = ARP_PTYPE_H;		//Protocol type	(IP)
		packet[3] = ARP_PTYPE_L;
		packet[4] = ARP_HLEN;			//Hardware address length
		packet[5] = ARP_PLEN;			//Protocol address length
		packet[6] = (oper>>8);			//Operation, 1 = request, 2 = reply
		packet[7] = (oper & 0xFF);
		
		memcpy(&packet[ARP_P_SRC_MAC], eth_mac_addr, ARP_HLEN);		//My MAC address
		memcpy(&packet[ARP_P_SRC_IP], eth_ip_addr, ARP_PLEN);		//My IP address
		
		memcpy(&packet[ARP_P_DEST_IP], dest_ip_addr, ARP_HLEN);		//Receiver's IP address
		
		if(oper == ARP_OPER_REPLY){
			memcpy(&packet[ARP_P_DEST_MAC], dest_mac_addr, ARP_HLEN);	//Receiver's MAC address
			eth_send(ARP_PACKET_SIZE);		//Send packet
		}else		
		if(oper == ARP_OPER_REQUEST){
			memset(&packet[ARP_P_DEST_MAC], 0, ARP_HLEN);					//Set address to zero if destination is unknown
			eth_send(ARP_PACKET_SIZE);		//Send packet		
		}
	
}
*/
