/*
 * tcp.c
 *
 * Created: 10.7.2011 18:55:09
 *  Author: Lauri L‰‰peri
 */ 

#include <string.h>
#include "network.h"
#include "tcp.h"
#include "ip.h"
#include "http.h"


#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lcd.h"
//sprintf_P(lcd_buf_l1, PSTR("Invalid ARP:"));
//sprintf_P(lcd_buf_l1, PSTR("code: %d"), err);
//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);

struct tcp_socket _sockets[TCP_MAX_CONN];
struct tcp_header _tcp_header;


/*
 *
 */
uint32_t rand_seq(){ 
	
	static uint32_t rand_seed = 12345678; /* Initial value */
	
	rand_seed = rand_seed * 1664525L + 1013904223L;
	return rand_seed;
}


/*
 * Return index of free socket of -1 if all sockets are in use
 */
int get_free_socket(){
	
	int i;
	for(i = 0; i < TCP_MAX_CONN; i++){
		if(_sockets[i].active == 0){
			return i;
		}
	}
	return -1;
}


/*
 * Get currently open socket. 
 * If source port is zero, socket comparison is done only on ip address 
 * (can be used to check if receiver already has an active socket).
 */
struct tcp_socket* get_socket(uint8_t* src_ip_addr, uint16_t src_port){
	
	int i;
	struct tcp_socket* socket;
	
	for(i = 0; i < TCP_MAX_CONN; i++){
		socket = &_sockets[i];
		
		//Skip if socket is not active
		if(!(socket->active)){
			continue;
		}
		
		if(src_port > 0){
			if(!memcmp(socket->dest_ip, src_ip_addr, 4) && (socket->dest_port == src_port)){
				return socket;
			}
		}else{
			if(!memcmp(socket->dest_ip, src_ip_addr, 4)){
				return socket;
			}
		}
	}
	return NULL;
}


/*
 *
 */
void init_socket(struct tcp_socket* socket, uint8_t* src_ip_addr, uint16_t src_port){
	socket->active = 0;
	socket->seq = 0;
	socket->ack = 0;
	socket->dest_port = src_port;
	memcpy(socket->dest_ip, src_ip_addr, 4);
}


/*
 * Calculate checksum over the whole packet, includind the payload.
 */
uint16_t tcp_chc(uint16_t len, const uint8_t* data, const uint8_t* dest_ip){
	
	uint32_t sum = 0;
	uint16_t chc = 0;
	
	//Pseudo header
	sum += (0xFFFF & (((_network_ip_addr[0] & 0xFF)<<8) | (_network_ip_addr[1] & 0xFF)));
	sum += (0xFFFF & (((_network_ip_addr[2] & 0xFF)<<8) | (_network_ip_addr[3] & 0xFF)));
	
	sum += (0xFFFF & (((dest_ip[0] & 0xFF)<<8) | (dest_ip[1] & 0xFF)));
	sum += (0xFFFF & (((dest_ip[2] & 0xFF)<<8) | (dest_ip[3] & 0xFF)));
	
	sum += IP_PROT_TCP;
	sum += len;
	
	//TCP header
	int i = 0;
	for(; i < (len - 1); i += 2){
		sum += (0xFFFF & (((data[i] & 0xFF)<<8) | (data[i + 1] & 0xFF)));
	}
	
	if(i < len){
		sum += ((data[i] & 0xFF)<<8);
	}
	
	while(sum>>16){
		sum = (sum & 0xFFFF) + (sum>>16);
	}
	
	chc = 0xFFFF ^ sum;
	
	return chc;
}


/*
 * Fill TCP header.
 * Return length of the packet
 */
/*
uint16_t tcp_fill_header(struct tcp_socket* socket, uint8_t type, uint16_t data_len){

	uint8_t* header = _network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	uint16_t pkt_len = TCP_HEADER_SIZE + data_len;	//Init length only to header
	
	header[TCP_H_SRC] = (TCP_PORT_HTTP >> 8);		//Source port (80)
	header[TCP_H_SRC + 1] = (TCP_PORT_HTTP & 0xFF);
	
	header[TCP_H_DST] = (socket->dest_port >> 8);	//Destination port
	header[TCP_H_DST + 1] = (socket->dest_port & 0xFF);	
	
	header[TCP_H_SEQ] = (socket->seq >> 24);
	header[TCP_H_SEQ + 1] = (socket->seq >> 16);
	header[TCP_H_SEQ + 2] = (socket->seq >> 8);
	header[TCP_H_SEQ + 3] = (socket->seq);
	
	header[TCP_H_ACK] = (socket->ack >> 24);
	header[TCP_H_ACK + 1] = (socket->ack >> 16);
	header[TCP_H_ACK + 2] = (socket->ack >> 8);
	header[TCP_H_ACK + 3] = (socket->ack);
	
	header[TCP_H_OFFSET] = TCP_OFFSET;			//Header length with no options
	
	//Set flags
	if(type == 1){ //SYN ACK
		header[TCP_H_OFFSET] = 0x60;			//Header length with one option
		header[TCP_H_FLAGS] = TCP_FLAG_SYN | TCP_FLAG_ACK;
		
		//uint16_t max_seg_size = ETH_BUF_SIZE - ETH_HEADER_SIZE - IP_HEADER_SIZE - TCP_HEADER_SIZE;
		header[TCP_H_OPT] = 0x02;
		header[TCP_H_OPT + 1] = 0x04;
		//header[TCP_H_OPT + 2] = (max_seg_size>>8);
		//header[TCP_H_OPT + 3] = (max_seg_size & 0xFF);
		header[TCP_H_OPT + 2] = (TCP_MAX_SEG_SIZE >> 8);
		header[TCP_H_OPT + 3] = (TCP_MAX_SEG_SIZE & 0xFF);
		
		socket->seq += 1;
		pkt_len += 4;	//Set packet length to contain only header and extra option
	}else
	if(type == 2){	//ACK
		header[TCP_H_FLAGS] = TCP_FLAG_ACK;
	}else
	if(type == 3){	//FIN
		header[TCP_H_FLAGS] = TCP_FLAG_ACK | TCP_FLAG_PSH | TCP_FLAG_FIN;
		socket->seq += 1;
	}else
	if(type == 4){
		header[TCP_H_FLAGS] = TCP_FLAG_PSH;
	}else
	if(type == 5){
		header[TCP_H_FLAGS] = TCP_FLAG_RST;
	}
	
	//uint16_t window_size = ETH_BUF_SIZE - ETH_HEADER_SIZE - IP_HEADER_SIZE - TCP_HEADER_SIZE;
	header[TCP_H_WSIZE] = TCP_WINDOW_SIZE >> 8;
	header[TCP_H_WSIZE + 1] = (TCP_WINDOW_SIZE & 0xFF);
	
	header[TCP_H_CHC] = 0x00;			//Zero checksum before calculation
	header[TCP_H_CHC + 1] = 0x00;
	
	header[TCP_H_URG] = 0x00;
	header[TCP_H_URG + 1] = 0x00;
	
	uint16_t chc = tcp_chc(pkt_len, header, socket->dest_ip);
	header[TCP_H_CHC] = (chc >> 8);						//Add valid checksum
	header[TCP_H_CHC + 1] = (chc & 0xFF);
	
	return pkt_len;
}
*/

/*
 *
 */
/*
uint32_t tcp_parse_seq(uint8_t* header){
	
	uint32_t seq = header[TCP_H_SEQ];
	seq <<= 8;
	seq |= header[TCP_H_SEQ + 1];
	seq <<= 8;
	seq |= header[TCP_H_SEQ + 2];
	seq <<= 8;
	seq |= header[TCP_H_SEQ + 3];
	return seq;
}
*/

/*
 *
 */
int tcp_parse_header(struct tcp_header* header, uint8_t* packet, uint16_t pkt_len){
	
	//Check that atleast header is received
	if(pkt_len < TCP_HEADER_SIZE){
		return -1;
	}
	
	//Ports
	header->src_port = (packet[TCP_H_SRC] << 8) | (packet[TCP_H_SRC + 1] & 0xFF);
	header->dst_port = (packet[TCP_H_DST] << 8) | (packet[TCP_H_DST + 1] & 0xFF);
	
	//Accept only packets to port 80
	if(header->dst_port != TCP_PORT_HTTP){
		return -1;
	}
	
	//Sequence number
	header->seqnum = packet[TCP_H_SEQ] & 0xFF;
	header->seqnum <<= 8;
	header->seqnum |= packet[TCP_H_SEQ + 1] & 0xFF;
	header->seqnum <<= 8;
	header->seqnum |= packet[TCP_H_SEQ + 2] & 0xFF;
	header->seqnum <<= 8;
	header->seqnum |= packet[TCP_H_SEQ + 3] & 0xFF;
	
	//Acknowledgement number
	header->acknum = packet[TCP_H_ACK] & 0xFF;
	header->acknum <<= 8;
	header->acknum |= packet[TCP_H_ACK + 1] & 0xFF;
	header->acknum <<= 8;
	header->acknum |= packet[TCP_H_ACK + 2] & 0xFF;
	header->acknum <<= 8;
	header->acknum |= packet[TCP_H_ACK + 3] & 0xFF;
	
	header->offset = ((packet[TCP_H_OFFSET] & 0xF0) >> 4);	//Size of the TCP header (32-bit words)
	header->flags = packet[TCP_H_FLAGS];
	header->window = (packet[TCP_H_WSIZE] << 8) | (packet[TCP_H_WSIZE + 1] & 0xFF);
	
	header->payload = packet + (header->offset * 4);
	header->payload_len = pkt_len - (header->offset * 4);
	
	return 0;
}



/*
 *
 */
void tcp_recv_syn(struct tcp_header* header, uint8_t* src_ip_addr){
	
	//Check that current endpoint has no other active sockets
	struct tcp_socket* socket = get_socket(src_ip_addr, 0);
	if(socket != NULL){
		return;
	}
	
	//Check for free sockets
	int socket_idx = get_free_socket();
	if(socket_idx < 0){
		return;
	}
	socket = &_sockets[socket_idx];
	
	//sprintf_P(lcd_buf_l1, PSTR("SEQ: %lu"), header->seqnum);
	//sprintf_P(lcd_buf_l2, PSTR("ACK: %lu"), header->acknum);
	//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
	socket->active = 1;	//Mark socket as active
	socket->handshake = 0;	//Mark handshake incomplete
	
	socket->seq = rand_seq();		//Reset sequence number
	
	socket->ack = header->seqnum + 1;	//Acknowledge SYN
	
	socket->dest_port = header->src_port;
	memcpy(socket->dest_ip, src_ip_addr, 4);
	
	tcp_send(socket, TCP_FLAG_SYN | TCP_FLAG_ACK, 0);	//Type 1, data length 0
}


/*
 *
 */
void tcp_recv_fin(struct tcp_header* header, uint8_t* src_ip_addr){
	
	//Get assosiated socket
	struct tcp_socket tmp_socket;
	struct tcp_socket* socket = get_socket(src_ip_addr, header->src_port);
	if(socket == NULL){	//If socket is not found create temporary from address and port
		init_socket(&tmp_socket, src_ip_addr, header->src_port);
		socket = &tmp_socket;
	}
	socket->seq = header->acknum;	//Accept all fin packets (Not good!)
	socket->ack = header->seqnum + 1;	//Copy seqnum to acknum and acknowledge FIN
	socket->active = 0;	//Mark socket as free
	tcp_send(socket, TCP_FLAG_FIN | TCP_FLAG_ACK, 0);	//Reply with type 3, no data
}


/*
 * Set socket inactive if it exists. No reply.
 */
void tcp_recv_rst(struct tcp_header* header, uint8_t* src_ip_addr){
	
	//Get assosiated socket
	struct tcp_socket* socket = get_socket(src_ip_addr, header->src_port);
	if(socket != NULL){
		socket->active = 0;
	}
}


/*
 *
 */
void tcp_recv(uint8_t* packet, uint16_t pkt_len, uint8_t* src_ip_addr){
	
	//Parse and check header
	if(tcp_parse_header(&_tcp_header, packet, pkt_len)){
		return;
	}
	
	//Handle SYN
	if(_tcp_header.flags & TCP_FLAG_SYN){	//New connection
		return tcp_recv_syn(&_tcp_header, src_ip_addr);
	}
	
	//Handle FIN
	if(_tcp_header.flags & TCP_FLAG_FIN){
		tcp_recv_fin(&_tcp_header, src_ip_addr);
		return;
	}
	
	//Handle RST
	if(_tcp_header.flags & TCP_FLAG_RST){
		tcp_recv_rst(&_tcp_header, src_ip_addr);
		return;
	}
	
	struct tcp_socket* socket = get_socket(src_ip_addr, _tcp_header.src_port);
	if(socket == NULL){
		return;
	}

	//Handshake finished
	if(!socket->handshake && (_tcp_header.acknum == (socket->seq + 1))){
		socket->handshake = 1;
		//sprintf_P(lcd_buf_l1, PSTR("Handshake: OK"));
		//lcd_write_buffer(lcd_buf_l1, NULL);
		return;
	}
	
	//Handle data packet
	if(_tcp_header.payload_len > 0){
		socket->seq += 1;
		socket->ack += _tcp_header.payload_len;
		tcp_send(socket, TCP_FLAG_ACK, 0);	//ACK data packet
		http_recv(_tcp_header.payload, _tcp_header.payload_len, socket);
		return;
	}
	
	//Reset if system missbehaves
	//tcp_send(socket, TCP_FLAG_RST, 0);
	
	/*
	static int i = 1;
	if(!(--i)){
		sprintf_P(lcd_buf_l1, PSTR("Len: %u"), _tcp_header.payload_len);
		snprintf(lcd_buf_l2, 16, (char*)_tcp_header.payload);
		lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	}
	*/
}


/*
 *	type, 1 = [SYN ACK], 2 = [ACK], 3 = [FIN], 4 = [PSH], 5 = [RST]
 */
void tcp_send(struct tcp_socket* socket, uint8_t flags, uint16_t pkt_len){
	
	//sprintf_P(lcd_buf_l1, PSTR("TCP:%u.%u.%u.%u"), socket->dest_ip[0], socket->dest_ip[1], socket->dest_ip[2], socket->dest_ip[3]);
	//lcd_write_buffer(lcd_buf_l1, NULL);
	
	uint8_t* packet = _network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	//uint16_t pkt_len = TCP_HEADER_SIZE + data_len;	//Init length only to header
	
	packet[TCP_H_SRC] = (TCP_PORT_HTTP >> 8);		//Source port (80)
	packet[TCP_H_SRC + 1] = (TCP_PORT_HTTP & 0xFF);
	
	packet[TCP_H_DST] = (socket->dest_port >> 8);	//Destination port
	packet[TCP_H_DST + 1] = (socket->dest_port & 0xFF);	
	
	packet[TCP_H_SEQ] = (socket->seq >> 24);
	packet[TCP_H_SEQ + 1] = (socket->seq >> 16);
	packet[TCP_H_SEQ + 2] = (socket->seq >> 8);
	packet[TCP_H_SEQ + 3] = (socket->seq);
	socket->seq += pkt_len;	//Increment sequence number
	
	packet[TCP_H_ACK] = (socket->ack >> 24);
	packet[TCP_H_ACK + 1] = (socket->ack >> 16);
	packet[TCP_H_ACK + 2] = (socket->ack >> 8);
	packet[TCP_H_ACK + 3] = (socket->ack);
	
	packet[TCP_H_OFFSET] = TCP_OFFSET;			//Header length with no options
	
	//Set flags
	packet[TCP_H_FLAGS] = flags;
	
	//uint16_t window_size = ETH_BUF_SIZE - ETH_HEADER_SIZE - IP_HEADER_SIZE - TCP_HEADER_SIZE;
	packet[TCP_H_WSIZE] = TCP_WINDOW_SIZE >> 8;
	packet[TCP_H_WSIZE + 1] = (TCP_WINDOW_SIZE & 0xFF);
	
	packet[TCP_H_CHC] = 0x00;			//Zero checksum before calculation
	packet[TCP_H_CHC + 1] = 0x00;
	
	packet[TCP_H_URG] = 0x00;
	packet[TCP_H_URG + 1] = 0x00;
	
	uint16_t chc = tcp_chc(pkt_len + TCP_HEADER_SIZE, packet, socket->dest_ip);
	packet[TCP_H_CHC] = (chc >> 8);						//Add valid checksum
	packet[TCP_H_CHC + 1] = (chc & 0xFF);
	
	ip_send(socket->dest_ip, IP_PROT_TCP, pkt_len + TCP_HEADER_SIZE);
}
