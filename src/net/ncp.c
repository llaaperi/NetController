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


static inline int reply_sensor_get_all(){
    
    char* reply = (char*)(_network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);
    
    int reply_len = sprintf_P(reply, PSTR("["));
    for(int i = 0; i < 3; i++){
        int temp = ds1820_get_cur(i);
        if(temp == DS1820_TEMP_NONE){
            continue;
        }
        
        reply_len += sprintf_P(reply + reply_len, PSTR("{'id':'%d','name':'sensor','value':'"), i);
        reply_len += ds1820_print_temp(reply + reply_len, temp);
        reply_len += sprintf_P(reply + reply_len, PSTR("'}%s"), (i==2)?"":",");
    }
    reply_len += sprintf_P(reply + reply_len, PSTR("]"));
    return reply_len;
}


static inline int reply_sensor_get(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 'a'){
        return reply_sensor_get_all();
    }
    return -1;
}

static inline int reply_sensor_set(uint8_t* packet, uint16_t pkt_len){
    return 0;
}

/**
 *
 */
static inline int reply_sensor(uint8_t* packet, uint16_t pkt_len){
    
    if(packet[0] == 'g'){
        return reply_sensor_get(packet + 2, pkt_len - 2);
    }
    if(packet[0] == 's'){
        return reply_sensor_set(packet + 2, pkt_len - 2);
    }
    return -1;
}


/**
 *
 */
static inline int reply_relay(uint8_t* packet, uint16_t pkt_len){
    
    char* reply = (char*)(_network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);
    int reply_len = 0;
    reply_len = sprintf_P(reply, PSTR("[{'id':'1','name':'relay','state':'true'}]"));
    //printData(reply, reply_len);
    return reply_len;
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
