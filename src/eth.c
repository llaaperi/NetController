/*
 * net.c
 *
 * Created: 7.7.2011 8:20:16
 *  Author: Lauri
 */ 

#include "eth.h"

volatile char eth_flag = 0;

static uint8_t dest_mac_addr[6];

void eth_init(uint8_t* mac_addr, uint8_t* ip_addr){
	
	eth_mac_addr = mac_addr;
	eth_ip_addr = ip_addr;
}


void eth_recv(){
	
	uint16_t pkt_len = 0;
	uint16_t pkt_type = 0;
	
	pkt_len = enc28j60_recv(ETH_BUF_SIZE, eth_buf);
	
	if(pkt_len != 0){
		
		memcpy(dest_mac_addr, &eth_buf[ETH_H_MAC_SRC], 6);
		
		pkt_type = (eth_buf[ETH_H_TYPE_H]<<8) | (eth_buf[ETH_H_TYPE_L] & 0xFF);

		if(pkt_type == ETH_TYPE_ARP){		//ARP
			arp_recv(&eth_buf[ETH_HEADER_SIZE]);				//Process packet
		}
		
		if(pkt_type == ETH_TYPE_IP){		//IP
			ip_recv(&eth_buf[ETH_HEADER_SIZE]);				//Process packet
		}
	}	
		
}


void eth_send(uint16_t len){
	
	//Add Ethernet header and send
	memcpy(&eth_buf[ETH_H_MAC_DEST], dest_mac_addr, 6);	//Destination MAC
	memcpy(&eth_buf[ETH_H_MAC_SRC], eth_mac_addr, 6);					//My MAC
	//eth_send_buf[ETH_H_TYPE_H] = eth_recv_buf[ETH_H_TYPE_H];				//Type high bits
	//eth_send_buf[ETH_H_TYPE_L] = eth_recv_buf[ETH_H_TYPE_L];				//Type low bits
	
	enc28j60_send((len + ETH_HEADER_SIZE), eth_buf);
	
}

ISR(INT1_vect){
	eth_flag = 1;
}