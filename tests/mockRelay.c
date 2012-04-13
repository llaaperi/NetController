#include "mockRelay.h"

void relay_init(){
}

void relay_switch(int card, int relay, int operation){
}

void relay_update(){
}

unsigned char relay_get_state(int card){
	return '0';
}

void relay_set_state(int card, unsigned char state){
}

unsigned char relay_get_ctrl(int card, int relay){
	return '0';
}
void relay_set_ctrl(int card, int relay, unsigned char ctrl){
}
void relay_set_ctrl_method(int card, int relay, unsigned char method){
}

int relay_get_sensor(int card, int relay){
	return 0;
}
void relay_set_sensor(int card, int relay, int value){
}

void relay_get_timer(int card, int relay, uint8_t type, uint8_t* hour, uint8_t* min){
}
void relay_set_timer(int card, int relay, uint8_t type, uint8_t hour, uint8_t min){
}

int relay_set_tag(uint8_t len, const char* tag, int card, int relay){
	return 0;
}

int relay_print_tag(char* buf, int card, int relay){
	return 0;
}
