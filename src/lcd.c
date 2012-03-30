/*
 * lcd.c
 *
 * Created: 12.6.2011 15:47:40
 *  Author: Lauri
 */ 


#include "lcd.h"


static inline void lcd_write_bits(char byte){
	
	LCD_DATA_PORT &= ~(LCD_DATA);				//Clear data port
	
	if(byte & (1<<0)){
		LCD_DATA_PORT |= LCD_D4;
	}
	if(byte & (1<<1)){
		LCD_DATA_PORT |= LCD_D5;
	}
	if(byte & (1<<2)){
		LCD_DATA_PORT |= LCD_D6;
	}
	if(byte & (1<<3)){
		LCD_DATA_PORT |= LCD_D7;
	}
	
	LCD_CTRL_PORT |= LCD_EN;
	LCD_CTRL_PORT &= ~LCD_EN;
	
}



/* 
* writes a command to LCD
*/
static inline void lcd_write_command(char data){
	
	LCD_DATA_PORT &= ~(LCD_DATA);			//Clear all related pins
	LCD_CTRL_PORT &= ~(LCD_EN | LCD_RS);

	lcd_write_bits(data>>4);				//Write four high bits first
	lcd_write_bits(data);					//Write low bits
	
	_delay_ms(5);							//Long delay for the command to take effect
	
}


/*
* writes ASCII data to LCD
*/
static inline void lcd_write_data(char data){
	

	LCD_DATA_PORT &= ~(LCD_DATA);			//Clear all related pins
	LCD_CTRL_PORT &= ~(LCD_EN | LCD_RS);
	
	LCD_CTRL_PORT |= LCD_RS; 				//RS = 1 for data

	lcd_write_bits(data>>4);				//Write four high bits first
	lcd_write_bits(data);					//Write low bits

	_delay_us(500);							//Short delay between data bytes

}


void lcd_init(){
	
	_delay_ms(100);										//Initial delay: wait for voltage to get up

	LCD_DATA_DDR |= LCD_DATA;							//Initializes the data direction register of 
	LCD_CTRL_DDR |= LCD_EN | LCD_RS | LCD_BL;			//data and control ports

	LCD_DATA_PORT &= ~(LCD_DATA);						//Clear all related pins
	LCD_CTRL_PORT &= ~(LCD_EN | LCD_RS | LCD_BL);
	
	lcd_write_bits(0x3);
	_delay_ms(10);
	lcd_write_bits(0x3);
	_delay_ms(1);
	lcd_write_bits(0x3);
	_delay_ms(1);
	
	
	lcd_write_bits(0x2);							//Function set two line mode
	
	lcd_write_bits(0x2);
	lcd_write_bits(0x8);
	_delay_ms(5);
	
	lcd_write_bits(0x0);
	lcd_write_bits(0xC);
	_delay_ms(5);
	
	lcd_write_bits(0x0);
	lcd_write_bits(0x1);
	_delay_ms(5);
	
	lcd_write_bits(0x0);
	lcd_write_bits(0x6);
	_delay_ms(5);
	
	_delay_ms(10);					//Extra delay after initialization
	
	/*
	lcd_write_command(0x28);									//Function set (Interface is 4 bits long. Specify the
																//number of display lines (2) and character font.)	
	lcd_write_command(0x0E);									//No cursor, no blink
	
	lcd_write_command(0x06);									//entry mode= move cursor right, no shift
	
	lcd_write_command(0x80);									//DDRAM address, start from 0
	*/
	
	//lcd_scroll_reset();										//Init static varables

}


void lcd_write_buffer(const char *buf1, const char *buf2){
	
	//lcd_clear();
	int i;
	
	if((buf1 != NULL) && (buf1[0] != 0)){
		
		lcd_write_command(0x80);						//DDRAM address 0 = first line;
		
		i = 0;
		while((buf1[i] != 0) && (i < 16)){				//Read string from the buffer to the NULL byte
			lcd_write_data(buf1[i++]);
		}
		while(i < 16){
			lcd_write_data(' ');
			i++;
		}
		
	}
	
	if((buf2 != NULL) && (buf2[0] != 0)){
		
		lcd_write_command(0x80 | 0x40);					//DDRAM address 0x40 = second line;
		
		i = 0;
		while((buf2[i] != 0) && (i < 16)){				//Read string from the buffer to the NULL byte
			lcd_write_data(buf2[i++]);
		}
		while(i < 16){
			lcd_write_data(' ');
			i++;
		}
	}
	
}

void lcd_clear(){
	lcd_write_command(0x01);
}

void lcd_backlight_on(){
	LCD_CTRL_PORT |= LCD_BL;
}

void lcd_backlight_off(){
	LCD_CTRL_PORT &= ~LCD_BL;
}

void lcd_backlight_toggle(){
	LCD_CTRL_PORT ^= LCD_BL;
}

