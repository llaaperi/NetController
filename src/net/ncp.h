/*
 * ncp.h
 *
 * NetController Protocol
 *
 * Created: 1.2.2014 12:50:30
 *  Author: Lauri L‰‰peri
 */ 


#ifndef NCP_H
#define NCP_H

#include <stdint.h>



/*
 *
 */
int ncp_recv(uint8_t* packet, uint16_t pkt_len);


#endif /* NCP_H */
