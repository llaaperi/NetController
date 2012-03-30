/*
 * net.h
 *
 * Created: 7.7.2011 8:19:33
 *  Author: Lauri
 */ 


#ifndef ETH_H
#define ETH_H

#include <avr/interrupt.h>
#include <string.h>
#include "enc28j60.h"
#include "arp.h"
#include "ip.h"

#include "lcd.h"


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

#define ETH_BUF_SIZE 1000

uint8_t eth_buf[ETH_BUF_SIZE];
uint8_t* eth_mac_addr;
uint8_t* eth_ip_addr;

void eth_init(uint8_t* my_mac, uint8_t* my_ip);

void eth_recv();
void eth_send(uint16_t len);

ISR(INT1_vect);

#endif /* NET_H_ */