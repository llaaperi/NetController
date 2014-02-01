/*
 * udp.c
 *
 * Created: 1.2.2014 12:51:09
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "udp.h"
#include "ncp.h"


#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lcd.h"

struct udp_socket* _socket;	//Current client socket

void printData(uint8_t* packet, uint16_t pkt_len){
    sprintf_P(lcd_buf_l1, PSTR("%s"), packet);
    sprintf_P(lcd_buf_l2, PSTR(""));
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}

/*
 *
 */
void ncp_recv(struct udp_socket* socket, uint8_t* packet, uint16_t pkt_len){
    printData(packet, pkt_len);
}


/*
 *
 */
void ncp_send(struct udp_socket* socket, uint8_t flags, uint16_t pkt_len){
}
