/*
 * tcp.c
 *
 * Created: 10.7.2011 18:55:09
 *  Author: Lauri L‰‰peri
 */ 

#include "tcp.h"

static uint32_t rand_seed = 12345678; /* Initial value */

uint8_t curr_socket = 0;
Tcp_socket socket[TCP_MAX_CONN];		//Only one client at any given time

uint32_t rand_seq(){ 
	rand_seed = rand_seed * 1664525L + 1013904223L;
	return rand_seed;
}

uint16_t tcp_chc(uint16_t len, const uint8_t* data, const uint8_t* dest_ip){
	
	uint32_t sum = 0;
	uint16_t chc = 0;
	
	//Pseudo header
	sum += (0xFFFF & (((eth_ip_addr[0] & 0xFF)<<8) | (eth_ip_addr[1] & 0xFF)));
	sum += (0xFFFF & (((eth_ip_addr[2] & 0xFF)<<8) | (eth_ip_addr[3] & 0xFF)));
	
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


void tcp_recv(uint16_t len, const uint8_t* packet, const uint8_t* src_ip_addr){
	
	
	uint8_t payload_idx = (packet[TCP_H_OFFSET]>>4) * 4;
	uint16_t payload_len = len - payload_idx;
	
	uint16_t src_port = (packet[TCP_H_SRC]<<8) | (packet[TCP_H_SRC + 1] & 0xFF);	
	uint16_t dest_port = (packet[TCP_H_DEST]<<8) | (packet[TCP_H_DEST + 1] & 0xFF);
	
	if(dest_port != TCP_PORT_HTTP){
		return;		//Ignore if wrong port
	}
	
	if(packet[TCP_H_FLAGS] & TCP_FLAG_SYN){	//New connection
		
		for(curr_socket = 0; curr_socket < TCP_MAX_CONN; curr_socket++){
			if(socket[curr_socket].active == 0){
				break;
			}
		}
		if(curr_socket == TCP_MAX_CONN){
			return;	//Too many connections
		}
		/*
		if(socket.active){
			return;		//One client already synced -> ignore
		}
		*/
		socket[curr_socket].active = 1;	//Mark socket as active
		
		socket[curr_socket].seq = rand_seq();		//Reset sequence number
		
		socket[curr_socket].ack = packet[TCP_H_SQ];
		socket[curr_socket].ack <<= 8;
		socket[curr_socket].ack |= packet[TCP_H_SQ + 1];
		socket[curr_socket].ack <<= 8;
		socket[curr_socket].ack |= packet[TCP_H_SQ + 2];
		socket[curr_socket].ack <<= 8;
		socket[curr_socket].ack |= packet[TCP_H_SQ + 3];
		socket[curr_socket].ack += 1;		//Acknowledge SYN
		
		socket[curr_socket].dest_port = src_port;
		memcpy(socket[curr_socket].dest_ip, src_ip_addr, 4);
		
		tcp_send(0, 1);		//No data, type 1
		return;
	}
	
	for(curr_socket = 0; curr_socket < TCP_MAX_CONN; curr_socket++){
		if(!memcmp(socket[curr_socket].dest_ip, src_ip_addr, 4) && (socket[curr_socket].dest_port == src_port)){
			break;
		}
	}
	if(curr_socket == TCP_MAX_CONN){
		return;	//Too many connections
	}
	
	/*
	if(memcmp(socket.dest_ip, src_ip_addr, 4) || (socket.dest_port != src_port)){
		return;		//Ignore packets that are not from current client
	}
	*/
	
	if(packet[TCP_H_FLAGS] & TCP_FLAG_FIN){
		socket[curr_socket].ack += 1;		//Acknowledge FIN
		tcp_send(0, 2);		//No data, type 2
		socket[curr_socket].active = 0;	//Mark socket as free
		return;
	}
	
	if(packet[TCP_H_FLAGS] & TCP_FLAG_RST){
		//tcp_ack += 1;		//Acknowledge RST
		//tcp_send(0, 2);		//No data, type 2
		socket[curr_socket].active = 0;	//Mark socket as free
		return;
	}
	
	if(payload_len != 0){
		socket[curr_socket].ack += payload_len;
		tcp_send(0, 2);			//ACK request packet
		http_recv(payload_len, (const char*)&packet[payload_idx]);
	}
	
}

/*
*	type, 1 = [SYN ACK], 2 = [ACK], 3 = [FIN], 4 = [PSH], 5 = [RST]
*/

void tcp_send(uint16_t len, uint8_t type){
	
	uint8_t* packet = eth_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	
	packet[TCP_H_SRC] = (TCP_PORT_HTTP>>8);			//Source port (80)
	packet[TCP_H_SRC + 1] = (TCP_PORT_HTTP & 0xFF);
	
	packet[TCP_H_DEST] = (socket[curr_socket].dest_port>>8); //Destination port
	packet[TCP_H_DEST + 1] = (socket[curr_socket].dest_port & 0xFF);	
	
	packet[TCP_H_SQ] = (socket[curr_socket].seq>>24);
	packet[TCP_H_SQ + 1] = (socket[curr_socket].seq>>16);
	packet[TCP_H_SQ + 2] = (socket[curr_socket].seq>>8);
	packet[TCP_H_SQ + 3] = (socket[curr_socket].seq);
	
	socket[curr_socket].seq += len;
	
	packet[TCP_H_ACK] = (socket[curr_socket].ack>>24);
	packet[TCP_H_ACK + 1] = (socket[curr_socket].ack>>16);
	packet[TCP_H_ACK + 2] = (socket[curr_socket].ack>>8);
	packet[TCP_H_ACK + 3] = (socket[curr_socket].ack);
	
	packet[TCP_H_OFFSET] = TCP_OFFSET;			//Header length with no options
	if(type == 1){ //SYN ACK
		
		packet[TCP_H_OFFSET] = 0x60;			//Header length with one option
		packet[TCP_H_FLAGS] = TCP_FLAG_SYN | TCP_FLAG_ACK;
		
		//uint16_t max_seg_size = ETH_BUF_SIZE - ETH_HEADER_SIZE - IP_HEADER_SIZE - TCP_HEADER_SIZE;
		packet[TCP_H_OPT] = 0x02;
		packet[TCP_H_OPT + 1] = 0x04;
		//packet[TCP_H_OPT + 2] = (max_seg_size>>8);
		//packet[TCP_H_OPT + 3] = (max_seg_size & 0xFF);
		packet[TCP_H_OPT + 2] = (TCP_MAX_SEG_SIZE>>8);
		packet[TCP_H_OPT + 3] = (TCP_MAX_SEG_SIZE & 0xFF);
		
		socket[curr_socket].seq += 1;
		len += 4;				//Increment len to contain extra option
	}else
	if(type == 2){	//ACK
		packet[TCP_H_FLAGS] = TCP_FLAG_ACK;
	}else
	if(type == 3){	//FIN
		packet[TCP_H_FLAGS] = TCP_FLAG_ACK | TCP_FLAG_PSH | TCP_FLAG_FIN;
		socket[curr_socket].seq += 1;
	}else
	if(type == 4){
		packet[TCP_H_FLAGS] = TCP_FLAG_PSH;
	}else
	if(type == 5){
		packet[TCP_H_FLAGS] = TCP_FLAG_RST;
	}
	
	//uint16_t window_size = ETH_BUF_SIZE - ETH_HEADER_SIZE - IP_HEADER_SIZE - TCP_HEADER_SIZE;
	packet[TCP_H_WSIZE] = TCP_WINDOW_SIZE>>8;
	packet[TCP_H_WSIZE + 1] = (TCP_WINDOW_SIZE & 0xFF);
	
	packet[TCP_H_CHC] = 0x00;			//Zero checksum before calculation
	packet[TCP_H_CHC + 1] = 0x00;
	
	packet[TCP_H_URG] = 0x00;
	packet[TCP_H_URG + 1] = 0x00;
	
	uint16_t chc = tcp_chc((len + TCP_HEADER_SIZE), packet, socket[curr_socket].dest_ip);
	packet[TCP_H_CHC] = (chc>>8);						//Add valid checksum
	packet[TCP_H_CHC + 1] = (chc & 0xFF);
		
	ip_send(len + TCP_HEADER_SIZE, socket[curr_socket].dest_ip);
	
}
