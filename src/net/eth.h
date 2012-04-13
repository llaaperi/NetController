/*
 * eth.h
 *
 * Created: 7.7.2011 8:19:33
 *  Author: Lauri
 */ 


#ifndef ETH_H
#define ETH_H

#define ETH_H_MAC_DEST 0
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

/*
 * Receive packet.
 * Return reply length
 */
uint16_t eth_recv(uint8_t* packet, uint16_t pkt_len);


/*
 *
 */
void eth_send(uint16_t len);

#endif /* eth_H_ */