/*
 * http.h
 *
 * Created: 10.7.2011 23:58:01
 *  Author: Lauri
 */ 


#ifndef HTTP_H
#define HTTP_H

#include <avr/pgmspace.h>
#include "NetController.h"
#include "tcp.h"
#include "lcd.h"
#include "ds1820.h"
#include "relay.h"

//const char FlashString[] PROGMEM = "This is a string ";

void http_recv(uint16_t len, const char* packet);


#endif /* HTTP_H */