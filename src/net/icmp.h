/*
 * icmp.h
 *
 * Created: 8.7.2011 17:17:22
 *  Author: Lauri L‰‰peri
 */ 


#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>

#define ICMP_H_TYPE 0
#define ICMP_H_CODE 1
#define ICMP_H_CHC 2
#define ICMP_H_ID 4
#define ICMP_H_SQ 6

#define ICMP_HEADER_SIZE 8

#define ICMP_TYPE_ECHOREPLY 0
#define ICMP_CODE_ECHOREPLY 0

#define ICMP_TYPE_ECHOREQUEST 8
#define ICMP_CODE_ECHOREQUEST 0

/*
 *
 */
uint16_t icmp_recv(uint8_t* packet, uint16_t pkt_len);

#endif /* ICMP_H */