#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mockEnc28j60.h"




void enc28j60_init(uint8_t* macaddr){
	_recvLen = 0;
	_sendLen = 0;
}


void enc28j60_send(uint16_t len, uint8_t* packet){
	
	_sendLen = len;
	memcpy(_sendBuffer, packet, len);
}


/*
 * Return 
 */
uint16_t enc28j60_recv(uint16_t maxlen, uint8_t* packet){
	
	uint16_t pktLen = _recvLen;
	if(maxlen < _recvLen){
		pktLen = maxlen;
	}
	
	memcpy(packet, _recvBuffer, _recvLen);
	
	return pktLen;
}

void print_recv(){
	printf("\n");
	for(int i = 0; i < _recvLen; i++){
		printf("%02x ", _recvBuffer[i]);
	}
	printf("\n");
}

void print_send(){
	printf("\n");
	for(int i = 0; i < _sendLen; i++){
		printf("%02x ", _sendBuffer[i]);
	}
	printf("\n");
}
