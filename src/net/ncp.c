/*
 * udp.c
 *
 * Created: 1.2.2014 12:51:09
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "network.h"
#include "udp.h"
#include "ncp.h"
#include "eth.h"
#include "ip.h"


#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "../lcd.h"
#include "../ds1820.h"
#include "../relay.h"
#include "../time.h"


/**
 *
 */
void printData(char* packet, uint16_t pkt_len){
    sprintf_P(lcd_buf_l1, PSTR("%s"), packet);
    sprintf_P(lcd_buf_l2, PSTR("len:%u"), pkt_len);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}

//{'id':'1','name':'Sensor','value':'1.0'}
int sensor_to_json(char* buf, int sensor){
    int len = 0;
    int temp = ds1820_get_cur(sensor);
    if(temp != DS1820_TEMP_NONE){
        len += sprintf_P(buf + len, PSTR("{'id':%d,'value':"), sensor);
        len += ds1820_print_temp(buf + len, temp);
        len += sprintf_P(buf + len, PSTR("}"));
    }
    return len;
}


int reply_sensor_get_all(){
    
    char* reply = (char*)(_network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);
    
    int reply_len = sprintf_P(reply, PSTR("["));
    int len = 0;
    for(int i = 0; i < 3; i++){
        len = sensor_to_json(reply + reply_len, i);
        reply_len += len;
        if((len > 0) && (i < 2)){
            reply_len += sprintf_P(reply + reply_len, PSTR(","));
        }
    }
    reply_len += sprintf_P(reply + reply_len, PSTR("]"));
    return reply_len;
}


int reply_sensor_get(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 'a'){
        return reply_sensor_get_all();
    }
    return -1;
}


int reply_sensor_set(uint8_t* packet, uint16_t pkt_len){
    return 0;
}

 
int reply_sensor(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 'g'){
        return reply_sensor_get(packet + 2, pkt_len - 2);
    }
    if(packet[0] == 's'){
        return reply_sensor_set(packet + 2, pkt_len - 2);
    }
    return -1;
}

int relay_to_json(char* buf, int card, int relay, unsigned char state){
    int len = 0;
    len += sprintf_P(buf + len, PSTR("{'id':%d,'state':'%s'}"), (card+1)*10+relay, (state>>relay & 0x01)?"true":"false");
    return len;
}

#define ACTIVE_RELAYS 8

int card_to_json(char* buf, int card){
    int len = 0;
    unsigned char state = relay_get_state(card);
    for(int i = 0; i < ACTIVE_RELAYS; i++){
        len += relay_to_json(buf + len, card, i, state);
        len += sprintf_P(buf + len, PSTR("%s"), (i<(ACTIVE_RELAYS-1))?",":"");
    }
    return len;
}

#define ACTIVE_CARDS 1

int reply_relay_get_all(){
    
    char* reply = (char*)(_network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);
    
    int reply_len = sprintf_P(reply, PSTR("["));
    int len = 0;
    for(int i = 0; i < ACTIVE_CARDS; i++){
        len = card_to_json(reply + reply_len, i);
        reply_len += len;
        if((len > 0) && (i < (ACTIVE_CARDS-1))){
            reply_len += sprintf_P(reply + reply_len, PSTR(","));
        }
    }
    reply_len += sprintf_P(reply + reply_len, PSTR("]"));
    return reply_len;
}

int reply_relay_get(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 'a'){
        return reply_relay_get_all();
    }
    return -1;
}


int reply_relay_set(uint8_t* packet, uint16_t pkt_len){
    
    int card = packet[0] - '0' - 1;
    int relay = packet[1] - '0';
    
    relay_switch(card, relay, RELAY_OP_TOGGLE);
    
    char* reply = (char*)(_network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);
    return sprintf_P(reply, PSTR("OK"));
}

 
int reply_relay(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 'g'){
        return reply_relay_get(packet + 2, pkt_len - 2);
    }
    if(packet[0] == 's'){
        return reply_relay_set(packet + 2, pkt_len - 2);
    }
    return -1;
}
 

/**
 *
 */
int ncp_recv(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 's'){
        return reply_sensor(packet + 2, pkt_len - 2);
    }
    
    if(packet[0] == 'r'){
        return reply_relay(packet + 2, pkt_len - 2);
    }
    
    return -1;  //No reply
}
