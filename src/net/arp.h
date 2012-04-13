/*
 * arp.h
 *
 * Created: 7.7.2011 12:20:58
 *  Author: Lauri
 */

#ifndef ARP_H
#define ARP_H

#define ARP_HTYPE_H 0x00
#define ARP_HTYPE_L 0x01
#define ARP_PTYPE_H 0x08
#define ARP_PTYPE_L 0x00
#define ARP_HLEN 6
#define ARP_PLEN 4
#define ARP_OPER_REQUEST 0x0001
#define ARP_OPER_REPLY 0x0002

#define ARP_P_HTYPE_H 0
#define ARP_P_HTYPE_L 1
#define ARP_P_PTYPE_H 2
#define ARP_P_PTYPE_L 3
#define ARP_P_HLEN 4
#define ARP_P_PLEN 5
#define ARP_P_OPER_H 6
#define ARP_P_OPER_L 7
#define ARP_P_SRC_MAC 8
#define ARP_P_SRC_IP 14
#define ARP_P_DEST_MAC 18
#define ARP_P_DEST_IP 24

#define ARP_PACKET_SIZE 28

#include <stdint.h>

//uint8_t arp_send_buf[42];					//ETH header + ARP packet

//#define ARP_TABLE_SIZE 5
//uint8_t arp_table[ARP_TABLE_SIZE * 10];		//Room for 5 address {ip1, mac1, ip2, mac2, ...}


uint16_t arp_recv(uint8_t* packet, uint16_t pkt_len);

//void arp_send(uint16_t oper, const uint8_t* dest_mac_addr, const uint8_t* dest_ip_addr);
//const uint8_t* arp_resolve(const uint8_t* ip_addr);


#endif /* ARP_H */
