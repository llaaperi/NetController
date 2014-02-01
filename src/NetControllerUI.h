/*
 * NetControllerUI.h
 *
 * Created: 11.6.2011 2:05:10
 *  Author: Lauri
 */ 


#ifndef NETCONTROLLERUI_H
#define NETCONTROLLERUI_H

#include <stdint.h>

uint8_t _display;

void ui_init();
void ui_key_pressed(char key);
void ui_refresh_display();


#endif /* NETCONTROLLERUI_H */