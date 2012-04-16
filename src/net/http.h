/*
 * http.h
 *
 * Created: 10.7.2011 23:58:01
 *  Author: Lauri
 */ 


#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>
#include "tcp.h"

//const char FlashString[] PROGMEM = "This is a string ";

void http_recv(uint8_t* packet, uint16_t pkt_len, struct tcp_socket* socket);


#endif /* HTTP_H */