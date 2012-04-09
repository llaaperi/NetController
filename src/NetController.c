/*
 * NetController.c
 *
 * Created: 10.6.2011 22:42:43
 *  Author: Lauri Lääperi
 */ 


#include "NetController.h"
#include "time.h"


uint8_t EEMEM eeprom_mac_addr[6] = {0x00, 0xFA, 0x20, 0xFA, 0x47, 0x31};
uint8_t EEMEM eeprom_ip_addr[4] = {10, 0, 0, 5};

volatile char lcd_timer_flag = 0;
volatile char tmp_timer_flag = 0;

//volatile uint16_t clock_ms = 0;
//volatile uint8_t clock_s = 0;
//volatile uint8_t clock_m = 0;
//volatile uint16_t clock_h = 0;

//volatile uint8_t time_s = 0;
//volatile uint8_t time_m = 0;
//volatile uint8_t time_h = 0;

//uint16_t timer_start = 0;
//uint16_t timer_end = 0;

uint8_t mac_addr[6] = {0,0,0,0,0,0};
uint8_t ip_addr[4] = {0,0,0,0};


void state(char led){
	PORTB &= ~(0x3);					//Clear LEDs
	PORTB |= (0x3 & led);				//Set LED
}


/*
void timer_ms_start(){
	timer_start = (clock_s * 1000) + clock_ms;
}


void timer_ms_stop(){
	timer_end = (clock_s * 1000) + clock_ms;
}


void timer_ms_print(){
	sprintf_P(lcd_buf_l1, PSTR("Elapsed time:"));
	sprintf_P(lcd_buf_l2, PSTR("%u ms"), timer_end - timer_start);
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}


int clock_print(char* buf){
	
	if(buf != NULL){
		return sprintf_P(buf, PSTR("%02u:%02u:%02u"), clock_h, clock_m, clock_s);
	}	
	return 0;
}

int time_print(char* buf){
	
	if(buf != NULL){
		return sprintf_P(buf, PSTR("%02u:%02u:%02u"), time_h, time_m, time_s);
	}	
	return 0;
}

void netcontroller_get_time(uint8_t* hour, uint8_t* min){
	*hour = time_h;
	*min = time_m;
}
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


static inline void disp_clock(){
	
	struct clockval cval;
	clock_get(&cval);
	struct timeval tval;
	time_get(&tval);
	sprintf_P(lcd_buf_l1, PSTR("Clock %02u:%02u:%02u"), cval.h, cval.m, cval.s);
	sprintf_P(lcd_buf_l2, PSTR("Time  %02u:%02u:%02u"), tval.h, tval.m, tval.s);
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
}


static inline void disp_addr(){
	
	sprintf_P(lcd_buf_l1, PSTR("%02x%02x%02x%02x%02x%02x"), mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	sprintf_P(lcd_buf_l2, PSTR("%d.%d.%d.%d"), ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
}

static inline void disp_default(){
	sprintf_P(lcd_buf_l1, PSTR("NetController"));
	sprintf_P(lcd_buf_l2, PSTR(" "));
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
}


void ui_control_menu(){
	
	char key = 0;
	
	sprintf_P(lcd_buf_l1, PSTR("Control menu:"));
	sprintf_P(lcd_buf_l2, PSTR("1=Addr, 2=Clock, 0=Cancel"));
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	
	do{
		key = keypad_get();
	}while(!((key >= '0') && (key <= '2')));
	
	if(key == '1'){
		
		sprintf_P(lcd_buf_l1, PSTR("Select address"));
		sprintf_P(lcd_buf_l2, PSTR("1=MAC, 2=IP"));
		lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		
		do{
			key = keypad_get();
		}while((key != '1') && (key != '2'));
		
		if(key == '1'){
			sprintf_P(lcd_buf_l1, PSTR("New MAC"));
			sprintf_P(lcd_buf_l2, PSTR(" "));
			lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		}else
		if(key == '2'){
			sprintf_P(lcd_buf_l1, PSTR("New IP"));
			sprintf_P(lcd_buf_l2, PSTR(" "));
			lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		}
			
	}else
	if(key == '2'){
		sprintf_P(lcd_buf_l1, PSTR("Set clock"));
		//sprintf_P(lcd_buf_l2, PSTR("%02u:%02u:%02u"), time_h, time_m, time_s);
		lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		
		unsigned int idx = 0;
		do{
			key = keypad_get();
			
			if((key >= '0') && (key <= '9')){
				
				lcd_buf_l2[idx++] = key;
				
				if(idx == 8){	//Stay on last character
					idx--;
				}
				
				if(idx == 2 || idx == 5){	//Jump over colons
					idx++;
				}
				
			}
			
			if(lcd_timer_flag){
				lcd_timer_flag = 0;
				lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
			}
			
		}while(key != 'A');
		
		unsigned int h = 0, m = 0, s = 0;
		sscanf_P(lcd_buf_l2, PSTR("%u:%u:%u"), &h, &m, &s);	//Read user given data
		
		if((h < 24) && (m < 60) && (s < 60)){	//Check that input is valid
			//time_h = h;
			//time_m = m;
			//time_s = s;	
		}
	}

}


int main(void)
{	
	//Load variables from EEPROM
	eeprom_read_block(mac_addr, eeprom_mac_addr, 6);
	eeprom_read_block(ip_addr, eeprom_ip_addr, 4);
	
	//Setup status LEDs
	DDRB  |= 0x3;
	PORTB |= 0x3;
	
	//TWI
	twi_init();
	
	//Relays
	relay_init();
	
	//Keypad
	keypad_init();
	
	//LCD
	lcd_init();
	lcd_backlight_on();
	
	//ENC28J60
	enc28j60_init(mac_addr);
	
	//Ethernet
	eth_init(mac_addr, ip_addr);
	
	//Init timer
	time_init(F_CPU);
	
	sei();	//Enable global interrupts
	
	state(OK);
	
	//relay_set_sensor(0,0,0,1,282);
	
	char key = 0, disp = 1;
	
	while(1){	//Main loop
		
		/*React to key presses*/
		if((key = keypad_get())){

			if(key == 'A'){
				//ui_relay_menu();
			}else
			if(key == 'B'){
				//ui_sensor_menu();
			}else
			if(key == 'C'){
				ui_control_menu();
			}else
			if(key == 'D'){		//Change LCD's content
				if(++disp > 3){	//Switch display content
					disp = 0;
				}
			}else
			if(key == '*'){
				lcd_backlight_toggle();
			}
			
			if((key >= '0') && (key <= '9')){
				
				if(key == '0'){
					relay_switch(RELAY_CARD1, RELAY_ALL, RELAY_OP_OFF);
				}else
				if(key == '9'){
					relay_switch(RELAY_CARD1, RELAY_ALL, RELAY_OP_ON);
				}else{
					relay_switch(RELAY_CARD1, key - 49, RELAY_OP_TOGGLE);	//Relays [0,7] = key [1,8]
				}
				
			}
			
		}
		
		/*React to new packets*/
		if(1){
			eth_recv();
		}
		
		/*Refresh LCD*/
		if(lcd_timer_flag){
			lcd_timer_flag = 0;			//Reset flag
			
			if(disp == 0){
				disp_default();
			}else
			if(disp == 1){
				disp_clock();
			}else
			if(disp == 2){
				disp_temp();
			}else
			if(disp == 3){
				disp_addr();
			}

		}		
		
		/*Refresh temperatures*/
		if(tmp_timer_flag){
			tmp_timer_flag = 0;
			ds1820_refresh_all();
		}
		
		/*Refresh relays*/
		relay_update();
		
	}
	
	return 0;
}
