#ifndef ENC28J60_H
#define ENC28J60_H

#include <stdint.h>


uint8_t _recvBuffer[1500];
uint8_t _sendBuffer[1500];
uint16_t _recvLen;
uint16_t _sendLen;


void enc28j60_init(uint8_t* macaddr);

void enc28j60_send(uint16_t len, uint8_t* packet);

uint16_t enc28j60_recv(uint16_t maxlen, uint8_t* packet);



void print_recv();
void print_send();

#endif
//@}
