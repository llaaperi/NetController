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
void ncp_recv(struct udp_socket* socket, uint8_t* packet, uint16_t pkt_len);


/*
 *
 */
void ncp_send(struct udp_socket* socket, uint8_t type, uint16_t pkt_len);


#endif /* NCP_H */
