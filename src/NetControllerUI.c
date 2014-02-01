/*
 * NetControllerUI.c
 *
 * Created: 10.6.2011 22:42:43
 *  Author: Lauri L‰‰peri
 */ 


#ifdef TEST
	#define sprintf_P sprintf
	#define PSTR(X) X
#else
	#include <avr/pgmspace.h>
#endif

#include <stdio.h>
#include "NetControllerUI.h"
#include "keypad.h"
#include "lcd.h"
#include "relay.h"
#include "time.h"
#include "net/network.h"

static inline void disp_refresh(){
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}


/*
 *
 */
static inline void disp_temp(){
	
	int idx = 0;
	
	sprintf_P(lcd_buf_l1, PSTR("Temp t1, t2, t3"));
	
	//idx += ds1820_tprint(lcd_buf_l2 + idx, ds1820_temps[0]);
	idx += sprintf_P(lcd_buf_l2 + idx, PSTR(" "));
	//idx += ds1820_tprint(lcd_buf_l2 + idx, ds1820_temps[1]);
	idx += sprintf_P(lcd_buf_l2 + idx, PSTR(" "));
	//idx += ds1820_tprint(lcd_buf_l2 + idx, ds1820_temps[2]);
	
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
}


/*
 *
 */
static inline void disp_clock(){
	
	struct clockval cval;
	clock_get(&cval);
	struct timeval tval;
	time_get(&tval);
	sprintf_P(lcd_buf_l1, PSTR("Clock %02u:%02u:%02u"), cval.h, cval.m, cval.s);
	sprintf_P(lcd_buf_l2, PSTR("Time  %02u:%02u:%02u"), tval.h, tval.m, tval.s);
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
}


/*
 *
 */
static inline void disp_addr(){
	
	sprintf_P(lcd_buf_l1, PSTR("%02x%02x%02x%02x%02x%02x"), _network_mac_addr[0], _network_mac_addr[1], _network_mac_addr[2], _network_mac_addr[3], _network_mac_addr[4], _network_mac_addr[5]);
	sprintf_P(lcd_buf_l2, PSTR("%d.%d.%d.%d"), _network_ip_addr[0], _network_ip_addr[1], _network_ip_addr[2], _network_ip_addr[3]);
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
}


/*
 *
 */
static inline void disp_default(){
	sprintf_P(lcd_buf_l1, PSTR("NetController"));
	sprintf_P(lcd_buf_l2, PSTR(" "));
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}

/*
 *
 */
void ui_init(){
    _display = 0;   //Default display
    //disp_default();
}


/*
 *
 */
void ui_refresh_display(){
	
	switch (_display){
		case 0:
			disp_default();
			break;
		case 1:
			disp_clock();
			break;
		case 2:
			disp_temp();
			break;
		case 3:
			disp_addr();
			break;

		default:
			disp_default();
			break;
	}
}

void change_addr_mac(){
    
    char key = 0;
    
    sprintf_P(lcd_buf_l1, PSTR("New MAC"));
    sprintf_P(lcd_buf_l2, PSTR("%02x%02x%02x%02x%02x%02x"), _network_mac_addr[0], _network_mac_addr[1], _network_mac_addr[2], _network_mac_addr[3], _network_mac_addr[4], _network_mac_addr[5]);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
    
    unsigned int idx = 0;
    do{
        key = keypad_get_input();   //Note that this is blockin function
        
        //TODO: implement a-f
        if((key >= '0') && (key <= '9')){
            
            lcd_buf_l2[idx++] = key;
            
            if(idx == 12){	//Stay on last character
                idx--;
            }
        }
        
        //Disp can be refreshed directly because keys are read with blocking function
        disp_refresh();
        
        //Cancel without changes
        if(key == 'C'){
            return;
        }
        
    }while(key != 'A');
    
    sscanf_P(lcd_buf_l2, PSTR("%2x%2x%2x%2x%2x%2x"), _network_mac_addr, _network_mac_addr+1, _network_mac_addr+2, _network_mac_addr+3, _network_mac_addr+4, _network_mac_addr+5);	//Read user given data
}


/**
 *
 */
void change_addr_ip(){
    
    char key = 0;
    
    sprintf_P(lcd_buf_l1, PSTR("New IP"));
    sprintf_P(lcd_buf_l2, PSTR("%03d.%03d.%03d.%03d"), _network_ip_addr[0], _network_ip_addr[1], _network_ip_addr[2], _network_ip_addr[3]);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
    
    unsigned int idx = 0;
    do{
        key = keypad_get_input();   //Note that this is blockin function
        
        if((key >= '0') && (key <= '9')){
            
            lcd_buf_l2[idx++] = key;
            
            if(idx == 15){	//Stay on last character
                idx--;
            }
            
            if(idx == 3 || idx == 7 || idx == 11){	//Jump over periods
                idx++;
            }
            
        }
        
        //Disp can be refreshed directly because keys are read with blocking function
        disp_refresh();
        
        //Cancel without changes
        if(key == 'C'){
            return;
        }
        
    }while(key != 'A');
    
    sscanf_P(lcd_buf_l2, PSTR("%u.%u.%u.%u"), _network_ip_addr, _network_ip_addr+1, _network_ip_addr+2, _network_ip_addr+3);	//Read user given data
    
}


/**
 *
 */
void change_addr(){
    
    char key = 0;
    
    sprintf_P(lcd_buf_l1, PSTR("Select address"));
    sprintf_P(lcd_buf_l2, PSTR("1=MAC, 2=IP"));
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
    
    key = keypad_get_input();
    
    if(key == '1'){
        change_addr_mac();
        return;
    }
    
    if(key == '2'){
        change_addr_ip();
        return;
    }
    
    //Cancel
    if(key == 'C'){
        return;
    }
}


/**
 *
 */
void change_time(){
    
    char key = 0;
    
    sprintf_P(lcd_buf_l1, PSTR("Set time"));
    sprintf_P(lcd_buf_l2, PSTR("%02u:%02u:%02u"), _time_h, _time_m, _time_s);
    lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
    
    unsigned int idx = 0;
    do{
        key = keypad_get_input();   //Note that this is blockin function
        
        if((key >= '0') && (key <= '9')){
            
            lcd_buf_l2[idx++] = key;
            
            if(idx == 8){	//Stay on last character
                idx--;
            }
            
            if(idx == 2 || idx == 5){	//Jump over colons
                idx++;
            }
            
        }
        
        //Disp can be refreshed directly because keys are read with blocking function
        disp_refresh();
        
        //Cancel without changes
        if(key == 'C'){
            return;
        }
        
    }while(key != 'A');
    
    unsigned int h = 0, m = 0, s = 0;
    sscanf_P(lcd_buf_l2, PSTR("%u:%u:%u"), &h, &m, &s);	//Read user given data
    
    if((h < 24) && (m < 60) && (s < 60)){	//Check that input is valid
        _time_h = h;
        _time_m = m;
        _time_s = s;
    }
}


/**
 *
 */
void ui_control_menu(){
	
	char key = 0;
	
	sprintf_P(lcd_buf_l1, PSTR("Control menu:"));
	sprintf_P(lcd_buf_l2, PSTR("1=Addr, 2=Clock, 0=Cancel"));
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
    key = keypad_get_input();
	
	if(key == '1'){
        //Support only IP changing for now
        change_addr_ip();
        //change_addr();
        return;
	}
    
    if(key == '2'){
        change_time();
        return;
    }
}


/**
 *
 */
void ui_key_pressed(char key){
    
	if(key == 'A'){
		//ui_relay_menu();
		return;
	}
	
	if(key == 'B'){
		//ui_sensor_menu();
		return;
	}
	
	if(key == 'C'){
		ui_control_menu();
		return;
	}
	
	if(key == 'D'){		//Change LCD's content
		if(++_display > 3){	//Switch display content
			_display = 0;
		}
		return;
	}
	
	if(key == '*'){
		lcd_backlight_toggle();
		return;
	}
	
	if((key >= '0') && (key <= '9')){
		
		if(key == '0'){
			relay_switch(RELAY_CARD1, RELAY_ALL, RELAY_OP_OFF);
			return;
		}
		
		if(key == '9'){
			relay_switch(RELAY_CARD1, RELAY_ALL, RELAY_OP_ON);
			return;
		}
		
		relay_switch(RELAY_CARD1, key - 49, RELAY_OP_TOGGLE);	//Relays [0,7] = key [1,8]
	}
}
