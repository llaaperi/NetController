#include "mockLcd.h"

/*
* Initializes the LCD
*/
void lcd_init(){
}

/*
* Control backlight of the LCD
*/
void lcd_backlight_on(){
}
void lcd_backlight_off(){
}
void lcd_backlight_toggle(){
}


/*
* Write selected string buffers to the screen.
* Only the first 16 characters are written if the buffer contains
* a string that is longer than that.
*/
void lcd_write_buffer(const char *buf1, const char *buf2){
}

void lcd_clear(){
}
