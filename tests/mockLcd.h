/*
 * lcd.h
 *
 * Created: 12.6.2011 15:47:53
 *  Author: Lauri
 */ 


#ifndef LCD_H
#define LCD_H

#define F_CPU 12500000UL

#define LCD_DATA_PORT PORTC
#define LCD_DATA_DDR DDRC
#define LCD_CTRL_PORT PORTD
#define LCD_CTRL_DDR DDRD

#define LCD_D4 (1<<PC3)
#define LCD_D5 (1<<PC2)
#define LCD_D6 (1<<PC1)
#define LCD_D7 (1<<PC0)
#define LCD_EN (1<<PD0)
#define LCD_RS (1<<PD1)
#define LCD_BL (1<<PD4)
#define LCD_DATA (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)

#define LCD_BUF_SIZE 32

#define LCD_REFRESH 200			//Refresh interval in milliseconds 0-65536

char lcd_buf_l1[LCD_BUF_SIZE];
char lcd_buf_l2[LCD_BUF_SIZE];

/*
* Initializes the LCD
*/
void lcd_init();

/*
* Control backlight of the LCD
*/
void lcd_backlight_on();
void lcd_backlight_off();
void lcd_backlight_toggle();


/*
* Write selected string buffers to the screen.
* Only the first 16 characters are written if the buffer contains
* a string that is longer than that.
*/
void lcd_write_buffer(const char *buf1, const char *buf2);

void lcd_clear();


#endif /* LCD_H */