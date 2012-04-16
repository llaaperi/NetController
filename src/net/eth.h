/*
 * eth.h
 *
 * Created: 7.7.2011 8:19:33
 *  Author: Lauri
 */ 


#ifndef ETH_H
#define ETH_H

#define ETH_H_MAC_DST 0
#define ETH_H_MAC_SRC 6
#define ETH_H_TYPE_H 12
#define ETH_H_TYPE_L 13

#define ETH_HEADER_SIZE 14

#define ETH_TYPE_IP 0x0800
#define ETH_TYPE_IP_H 0x08
#define ETH_TYPE_IP_L 0x00
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_ARP_H 0x08
#define ETH_TYPE_ARP_L 0x06

#include <stdint.h>

struct eth_header{
	uint8_t dst_mac[6];
	uint8_t src_mac[6];
	uint16_t type;
	uint8_t* payload;
	uint16_t payload_len;
};


/*
 * Receive packet.
 */
void eth_recv(uint8_t* packet, uint16_t pkt_len);


/*
 *
 */
void eth_send(const uint8_t* dst_mac_addr, uint16_t type, uint16_t len);

#endif /* eth_H_ */