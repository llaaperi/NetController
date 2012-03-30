/*
 * arp.c
 *
 * Created: 7.7.2011 12:20:46
 *  Author: Lauri L‰‰peri
 */ 

#include "arp.h"

void arp_recv(const uint8_t* packet){
	
	uint16_t arp_oper = 0;
	
	if((packet[ARP_P_HTYPE_H] != ARP_HTYPE_H) || (packet[ARP_P_HTYPE_L] != ARP_HTYPE_L)){		//Wrong hardware type
		return;
	}
	
	if((packet[ARP_P_PTYPE_H] != ARP_PTYPE_H) || (packet[ARP_P_PTYPE_L] != ARP_PTYPE_L)){		//Wrong protocol type
		return;
	}
	
	if(packet[ARP_P_HLEN] != ARP_HLEN){					//Wrong hardware address length
		return;
	}
	
	if(packet[ARP_P_PLEN] != ARP_PLEN){					//Wrong protocol address length
		return;
	}
	
	if(memcmp(eth_ip_addr, &packet[ARP_P_DEST_IP], ARP_PLEN)){			//Wrong destination IP
		return;
	}
	
	arp_oper = (packet[ARP_P_OPER_H]<<8) | (packet[ARP_P_OPER_L] & 0xFF);
	
	if(arp_oper == ARP_OPER_REPLY){		//ARP reply
		return;
	}	
	
	if(arp_oper == ARP_OPER_REQUEST){	//ARP request
		
		uint8_t* reply = eth_buf + ETH_HEADER_SIZE;
		
		memcpy(reply, packet, 7);		//First 7 bytes are the same for response
		
		reply[ARP_P_OPER_L] = (ARP_OPER_REPLY & 0xFF);
		
		memcpy(&reply[ARP_P_DEST_MAC], &packet[ARP_P_SRC_MAC], ARP_HLEN);	//Receiver's MAC address
		memcpy(&reply[ARP_P_DEST_IP], &packet[ARP_P_SRC_IP], ARP_HLEN);		//Receiver's IP address
		memcpy(&reply[ARP_P_SRC_MAC], eth_mac_addr, ARP_HLEN);		//My MAC address
		memcpy(&reply[ARP_P_SRC_IP], eth_ip_addr, ARP_PLEN);		//My IP address
		
		eth_send(ARP_PACKET_SIZE);				//Send ARP reply
	}	

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
