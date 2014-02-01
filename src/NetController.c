/*
 * NetController.c
 *
 * Created: 10.6.2011 22:42:43
 *  Author: Lauri Lääperi
 */ 


//#define F_CPU 6250000UL
//#define F_CPU 8000000UL
#define F_CPU 12500000UL

#define ERROR 0x1
#define OK 0x2

#include "NetControllerUI.h"
#include "net/network.h"
#include "keypad.h"
#include "lcd.h"
#include "relay.h"
#include "ds1820.h"
#include "time.h"


void state(char led){
	PORTB &= ~(0x3);					//Clear LEDs
	PORTB |= (0x3 & led);				//Set LED
}


int main(void){
	
	//Setup status LEDs
	DDRB  |= 0x3;	//Led pins as output
	PORTB |= 0x3;	//Leds on
	
	//TWI
	twi_init(F_CPU);
	
	//Init timer
	time_init(F_CPU);
	
	//Network
	network_init();
	
	//Relays
	relay_init();
	
	//Keypad
	keypad_init();
	
	//LCD
	lcd_init();
	lcd_backlight_on();
	
	sei();	//Enable global interrupts
	
	state(OK);
	
    //Init UI
	ui_init();
    
    //relay_set_sensor(0,0,0,1,282);
	
	char key = 0;
	struct clockval lcd_timer, tmp_timer;
	timer_start(&lcd_timer);
	timer_start(&tmp_timer);
	
	while(1){	//Main loop
		
		/*Receive new packets*/
		network_recv();
		
		/*Refresh relays*/
		relay_update();
		
		/*React to key presses*/
		if((key = keypad_get())){
			ui_key_pressed(key);
		}
		
		/*Refresh temperatures*/
		if(timer_get_elapsed(&tmp_timer) >= 1000){	//Refresh temps every second
			ds1820_refresh_all();
			timer_start(&tmp_timer);	//Reset temperature timer
		}
		
		/*Refresh LCD*/
		if(timer_get_elapsed(&lcd_timer) >= 200){	//Refresh LCD every 200ms
			//ui_refresh_display();
			timer_start(&lcd_timer);	//Reset LCD timer
		}
	}
	
	return 0;
}
