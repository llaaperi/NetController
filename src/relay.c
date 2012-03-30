/* relay.c
 *
 * Created: 15.6.2011 16:31:30
 *  Author: Lauri
 */ 

#include "relay.h"

uint8_t relay_state[3] = {0, 0, 0};	//Current state bytes of the relay cards

//bit 0 = manual, bit 1 = sensor, bit 2 = time, bit 3 = sensor on/off, bits 4-7 sensor id
uint8_t EEMEM eeprom_relay_ctrl[3][8] = {{RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL},
										 {RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL},
										 {RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL, RELAY_CTRL_MANUAL}};		//Control bytes of the relay cards

uint16_t EEMEM eeprom_relay_sensor[3][8];	//Sensor value with the control bit 3 and 4-7 defines the relay behavior
uint16_t EEMEM eeprom_relay_timer_start[3][8];
uint16_t EEMEM eeprom_relay_timer_end[3][8];

//uint8_t EEMEM eeprom_relay_tags[384] = {0};	//One buffer for all tags, 16 bytes / tag (16*8*3=384)

char EEMEM eeprom_relay_tag11[RELAY_TAG_SIZE] = "Relay1";	//Tags for card 1
char EEMEM eeprom_relay_tag12[RELAY_TAG_SIZE] = "Relay2";
char EEMEM eeprom_relay_tag13[RELAY_TAG_SIZE] = "Relay3";
char EEMEM eeprom_relay_tag14[RELAY_TAG_SIZE] = "Relay4";
char EEMEM eeprom_relay_tag15[RELAY_TAG_SIZE] = "Relay5";
char EEMEM eeprom_relay_tag16[RELAY_TAG_SIZE] = "Relay6";
char EEMEM eeprom_relay_tag17[RELAY_TAG_SIZE] = "Relay7";
char EEMEM eeprom_relay_tag18[RELAY_TAG_SIZE] = "Relay8";

char EEMEM eeprom_relay_tag21[RELAY_TAG_SIZE] = "Relay1";	//Tags for card 2
char EEMEM eeprom_relay_tag22[RELAY_TAG_SIZE] = "Relay2";
char EEMEM eeprom_relay_tag23[RELAY_TAG_SIZE] = "Relay3";
char EEMEM eeprom_relay_tag24[RELAY_TAG_SIZE] = "Relay4";
char EEMEM eeprom_relay_tag25[RELAY_TAG_SIZE] = "Relay5";
char EEMEM eeprom_relay_tag26[RELAY_TAG_SIZE] = "Relay6";
char EEMEM eeprom_relay_tag27[RELAY_TAG_SIZE] = "Relay7";
char EEMEM eeprom_relay_tag28[RELAY_TAG_SIZE] = "Relay8";

char EEMEM eeprom_relay_tag31[RELAY_TAG_SIZE] = "Relay1";	//Tags for card 3
char EEMEM eeprom_relay_tag32[RELAY_TAG_SIZE] = "Relay2";
char EEMEM eeprom_relay_tag33[RELAY_TAG_SIZE] = "Relay3";
char EEMEM eeprom_relay_tag34[RELAY_TAG_SIZE] = "Relay4";
char EEMEM eeprom_relay_tag35[RELAY_TAG_SIZE] = "Relay5";
char EEMEM eeprom_relay_tag36[RELAY_TAG_SIZE] = "Relay6";
char EEMEM eeprom_relay_tag37[RELAY_TAG_SIZE] = "Relay7";
char EEMEM eeprom_relay_tag38[RELAY_TAG_SIZE] = "Relay8";

//Store tag pointers to the two-dimensional array for easy indexing
char* EEMEM eeprom_relay_tags[3][8] = {{eeprom_relay_tag11, eeprom_relay_tag12, eeprom_relay_tag13, eeprom_relay_tag14, eeprom_relay_tag15, eeprom_relay_tag16, eeprom_relay_tag17, eeprom_relay_tag18},
										{eeprom_relay_tag21, eeprom_relay_tag22, eeprom_relay_tag23, eeprom_relay_tag24, eeprom_relay_tag25, eeprom_relay_tag26, eeprom_relay_tag27, eeprom_relay_tag28},
										{eeprom_relay_tag31, eeprom_relay_tag32, eeprom_relay_tag33, eeprom_relay_tag34, eeprom_relay_tag35, eeprom_relay_tag36, eeprom_relay_tag37, eeprom_relay_tag38}};


static char card_to_addr(int card){

	char addr = 0;
	
	switch (card) {
		case 0: addr = RELAY_ADDR1; break;
		case 1: addr = RELAY_ADDR2; break;
		case 2: addr = RELAY_ADDR3; break;
		default: addr = 0;
	}
	
	return addr;

}


void relay_init(){
	/*
	uint8_t state = 0;
	//Restore relay card state
	for(int i = 0; i < 3; i++){	
		state = eeprom_read_byte(eeprom_relay_state + i);
		relay_set_state(i, state);
	}
	*/
	relay_update();
}


void relay_switch(int card, int relay, int operation){
	
	char addr = card_to_addr(card);
	
	if(addr != 0){
		
		uint8_t ctrl = eeprom_read_byte(&eeprom_relay_ctrl[card][relay]);
		
		switch(operation){
			case RELAY_OP_TOGGLE: ctrl ^= RELAY_CTRL_STATE; break;
			case RELAY_OP_OFF: ctrl &= ~RELAY_CTRL_STATE; break;
			case RELAY_OP_ON: ctrl |= RELAY_CTRL_STATE; break;
		}
		
		eeprom_write_byte(&eeprom_relay_ctrl[card][relay], ctrl);
		
		/*
		uint8_t bitmask = 0;
		uint8_t state =  eeprom_read_byte(eeprom_relay_state + card);
		
		if(relay == RELAY_ALL){
			bitmask = 0xFF;
		}else
		if((relay >= 0) && (relay <= 7)){
			bitmask = (1<<relay);
		}
		
		switch(operation){
			case RELAY_OP_TOGGLE: state ^= bitmask; break;
			case RELAY_OP_OFF: state &= ~bitmask; break;
			case RELAY_OP_ON: state |= bitmask; break;
		}
		*/
		/*
		sprintf(lcd_buf_l1, "Relay state:");
		sprintf(lcd_buf_l2, "0x%02x", relay_state[card - 1]);
		lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		*/
		//eeprom_write_byte(eeprom_relay_state + card, state);	//Update state to the eeprom
		//twi_write_byte(addr, state);								//Set state of the card
	
	}
	
}


unsigned char relay_get_state(int card){
	
	if((card >= 0) && (card <= 2)){
		//return eeprom_read_byte(eeprom_relay_state + card);
		return relay_state[card];
	}
	return 0;

}


void relay_set_state(int card, unsigned char state){
	
	char addr = card_to_addr(card);
	
	if(addr != 0){
		//eeprom_write_byte(eeprom_relay_state + card, state);	//Update state to the eeprom
		relay_state[card] = state;
		twi_write_byte(addr, state);								//Set state of the card
	}
	
}


unsigned char relay_get_ctrl(int card, int relay){
	
	if((card >= 0) && (card <= 2)){
		return eeprom_read_byte(&eeprom_relay_ctrl[card][relay]);
	}
	return 0;
}

void relay_set_ctrl(int card, int relay, unsigned char ctrl){
	
	char addr = card_to_addr(card);
	
	if(addr != 0){
		eeprom_write_byte(&eeprom_relay_ctrl[card][relay], ctrl);		//Update relay control to the eeprom
	}
	
}

void relay_set_ctrl_method(int card, int relay, unsigned char method){
	
	char addr = card_to_addr(card);
	
	if(addr != 0){
		
		uint8_t ctrl = eeprom_read_byte(&eeprom_relay_ctrl[card][relay]);
		
		ctrl &= 0xF8;	//Clear control method bits (0-2)
		ctrl |= (method & 0x07);	//Change control method but keep other information
		
		sprintf(lcd_buf_l1, "Method 0x%x", method);
		sprintf(lcd_buf_l2, "Ctrl 0x%x", ctrl);
		lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		
		eeprom_write_byte(&eeprom_relay_ctrl[card][relay], ctrl);		//Update relay control to the eeprom
	}
}

int relay_set_tag(uint8_t len, const char* tag, int card, int relay){
	
	if((card < 0) && (card > 2)){
		return 0;
	}
	
	if((relay < 0) && (relay > 7)){
		return 0;
	}	
	
	if(len > RELAY_TAG_SIZE){
		len = RELAY_TAG_SIZE;
	}
	
	uint8_t* eeprom_str = (uint8_t *)eeprom_read_word((void*)&eeprom_relay_tags[card][relay]);
	
	eeprom_write_block(tag, eeprom_str, len);
	
	if(len < DS1820_TAG_SIZE){
		eeprom_write_byte(eeprom_str + len, 0);
	}
	
	return len;
}

int relay_print_tag(char* buf, int card, int relay){
	
	char c;
	uint8_t* eeprom_str;
	int idx = 0;
	
	if(buf != NULL){
		
		eeprom_str = (uint8_t *)eeprom_read_word((void *)&eeprom_relay_tags[card][relay]);
		
		while((c = eeprom_read_byte(eeprom_str++)) && (idx < RELAY_TAG_SIZE)){
			buf[idx++] = c;
		}
		
	}	
	return idx;
}

void relay_update(){
	
	int sensor = 0, val = 0, cur_val = 0;
	uint8_t state = 0, ctrl = 0;
	
	for(int card = 0; card < 3; card++){
		
		//state = eeprom_read_byte(eeprom_relay_state + card);	//Update state to the eeprom
		
		for(int relay = 0; relay < 8; relay++){
			
			ctrl = eeprom_read_byte(&eeprom_relay_ctrl[card][relay]);
			
			
			if(ctrl & RELAY_CTRL_MANUAL){
				if(ctrl & RELAY_CTRL_STATE){	//If bit 3 is set in the control then relay should be on
					state |= (1<<relay);	
				}
			}else
			if(ctrl & RELAY_CTRL_SENSOR){
				
				sensor = (ctrl>>4);
				
				val = (int)eeprom_read_word(&eeprom_relay_sensor[card][relay]);
				cur_val = ds1820_get_cur(sensor);
				
				if(cur_val == DS1820_TEMP_NONE || cur_val == DS1820_TEMP_INIT){
					continue;	//Do not alter relay state if sensor has none or initial value
				}
				
				if(cur_val > val){	//If current sensor value is greater than switching limit
					
					//sprintf(lcd_buf_l1, "Current %d", cur_val);
					//sprintf(lcd_buf_l2, "Value %d", val);
					//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
					
					if(ctrl & RELAY_CTRL_STATE){	//Turn on
						state |= (1<<relay);
					}else{							//Turn off
						state &= ~(1<<relay);
					}
					
				}else{
					
					//sprintf(lcd_buf_l1, "Value %d", val);
					//sprintf(lcd_buf_l2, "Current %d", cur_val);
					//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
					
					if(ctrl & RELAY_CTRL_STATE){
						state &= ~(1<<relay);
					}else{
						state |= (1<<relay);
					}
					
				}
				
			}else
			if(ctrl & RELAY_CTRL_TIMER){
				
				uint8_t start_h = 0, start_m = 0, end_h = 0, end_m = 0;
				relay_get_timer(card, relay, RELAY_TIMER_START, &start_h, &start_m);
				relay_get_timer(card, relay, RELAY_TIMER_END, &end_h, &end_m);
				
				uint16_t start = start_h;
				start <<= 8;
				start += start_m;
				
				uint16_t end = end_h;
				end <<= 8;
				end += end_m;
				
				uint8_t time_h = 0, time_m = 0;
				netcontroller_get_time(&time_h, &time_m);
				
				uint16_t time = time_h;
				time <<= 8;
				time += time_m;
				
				//sprintf(lcd_buf_l1, "Start %u, End %u", start, end);
				//sprintf(lcd_buf_l2, "Time %u", time);
				//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
				
				if(end > start){
					
					if((time >= start) && (time < end)){
						
						if(ctrl & RELAY_CTRL_STATE){
							state |= (1<<relay);
						}else{
							state &= ~(1<<relay);
						}
						
					}else{
						
						if(ctrl & RELAY_CTRL_STATE){
							state &= ~(1<<relay);
						}else{
							state |= (1<<relay);
						}
					}
				}else{
					
					if((time >= start) && (time < 6144)){	//6144 = 24:00
						//Between
						if(ctrl & RELAY_CTRL_STATE){
							state |= (1<<relay);
						}else{
							state &= ~(1<<relay);
						}
					}else
					if((time >= 0) && (time < end)){
						//Between
						if(ctrl & RELAY_CTRL_STATE){
							state |= (1<<relay);
						}else{
							state &= ~(1<<relay);
						}
					}else{
						//Outside
						if(ctrl & RELAY_CTRL_STATE){
							state &= ~(1<<relay);
						}else{
							state |= (1<<relay);
						}
					}
					
				}
				
				
			}
			
		}
		
		relay_set_state(card, state);								//Set state of the card
		
	}
	
}

void relay_set_sensor(int card, int relay, int value){
	
	if((card >= 0) && (card <= 2)){
		eeprom_write_word(&eeprom_relay_sensor[card][relay], value);	//Update sensor value to the eeprom
	}
}

int relay_get_sensor(int card, int relay){
	
	if((card >= 0) && (card <= 2)){
		return (int)eeprom_read_word(&eeprom_relay_sensor[card][relay]);
	}
	return 0;
}


void relay_get_timer(int card, int relay, uint8_t type, uint8_t* hour, uint8_t* min){
	
	if((card < 0) && (card > 2)){
		return;
	}
	
	if((relay < 0) && (relay > 7)){
		return;
	}
	
	uint16_t time = 0;
	
	if(type == RELAY_TIMER_START){
		time = eeprom_read_word(&eeprom_relay_timer_start[card][relay]);
	}else{
		time = eeprom_read_word(&eeprom_relay_timer_end[card][relay]);
	}
	
	*hour = (time>>8);
	*min = (time&0xFF);
	
}


void relay_set_timer(int card, int relay, uint8_t type, uint8_t hour, uint8_t min){
	
	if((card < 0) && (card > 2)){
		return;
	}
	
	if((relay < 0) && (relay > 7)){
		return;
	}
	
	if((hour > 23) || (min > 59)){
		return;
	}
	
	uint16_t time = hour;
	time <<= 8;
	time += min;
	
	if(type == RELAY_TIMER_START){
		eeprom_write_word(&eeprom_relay_timer_start[card][relay], time);
	}else{
		eeprom_write_word(&eeprom_relay_timer_end[card][relay], time);
	}
	
}

