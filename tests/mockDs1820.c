#include "mockDs1820.h"

int ds1820_get_cur(int sensor){
	return 0;
}

int ds1820_get_min(int sensor){
	return 0;
}

int ds1820_get_max(int sensor){
	return 0;
}

int ds1820_convert(int sensor){
	return 0;
}

int ds1820_convert_ready(int sensor){
	return 0;
}

int ds1820_get_temp(int sensor){
	return 0;
}

void ds1820_refresh_all(){
}

void ds1820_reset_temp(int sensor){
}

int ds1820_set_tag(uint8_t len, const char* tag, int sensor){
	return 0;
}

int ds1820_print_temp(char* buf, int temp){
	return 0;
}

int ds1820_print_tag(char* buf, int sensor){
	return 0;
}
