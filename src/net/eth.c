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


/*
 *
 */
uint16_t eth_recv(uint8_t* packet, uint16_t pkt_len){
	
	uint16_t pkt_type = 0, reply_len = 0;
	
	if(pkt_len < ETH_HEADER_SIZE){	//Check that at least header is received
		return 0;
	}
		
	//Parse packet type
	pkt_type = (packet[ETH_H_TYPE_H]<<8) | (packet[ETH_H_TYPE_L] & 0xFF);
	
	//Switch next handler
	switch(pkt_type){
		case ETH_TYPE_ARP:
			reply_len = arp_recv(&packet[ETH_HEADER_SIZE], pkt_len - ETH_HEADER_SIZE);
			break;
		case ETH_TYPE_IP:
			reply_len = ip_recv(&packet[ETH_HEADER_SIZE], pkt_len - ETH_HEADER_SIZE);
		default:
			break;
	}
	
	if(reply_len == 0){
		return 0;
	}
	
	//Reply
	memcpy(&packet[ETH_H_MAC_DEST], &packet[ETH_H_MAC_SRC], 6);	//src MAC to dest MAC
	memcpy(&packet[ETH_H_MAC_SRC], _network_mac_addr, 6);	//My MAC to src
	return reply_len + ETH_HEADER_SIZE;
}


/*
 *
 */
void eth_send(uint16_t len){
	
	/*
	//Add Ethernet header and send
	memcpy(&eth_buf[ETH_H_MAC_DEST], dest_mac_addr, 6);	//Destination MAC
	memcpy(&eth_buf[ETH_H_MAC_SRC], eth_mac_addr, 6);					//My MAC
	//eth_send_buf[ETH_H_TYPE_H] = eth_recv_buf[ETH_H_TYPE_H];				//Type high bits
	//eth_send_buf[ETH_H_TYPE_L] = eth_recv_buf[ETH_H_TYPE_L];				//Type low bits
	
	enc28j60_send((len + ETH_HEADER_SIZE), eth_buf);
	*/
}