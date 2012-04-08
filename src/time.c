/*
 * time.c
 *
 * Created: 10.7.2011 18:55:09
 *  Author: Lauri L‰‰peri
 */ 

#ifdef TEST
	#define sprintf_P sprintf
	#define PSTR(X) X
#else
	#include <avr/pgmspace.h>
#endif

#include <stdio.h>
#include "time.h"


/*
 *
 */
void time_init(uint32_t f_cpu){

	//Initialize clock variables
	_clock_h = 0;
	_clock_m = 0;
	_clock_s = 0;
	_clock_ms = 0;
	
	//Initialize time variables
	_time_h = 0;
	_time_m = 0;
	_time_s = 0;
	
	
	#ifndef TEST	
	//Initialize timer if F_CPU is 6.25MHz
	if(f_cpu == 6250000){
		TCCR1B = (1<<WGM12) | (1<<CS10);	//CTC mode, prescaling 1 (6250000 timer pulses / second)
		OCR1A = 6249;						//Interrupt every millisecond (6250 pulses / ms)
		TIMSK1 |= (1<<OCIE1A);				//Enable interrupt	
		return;
	}
	
	//Clock is 12.5MHz
	if(f_cpu == 12500000){
		TCCR1B = (1<<WGM12) | (1<<CS10);	//CTC mode, prescaling 1 (12500000 timer pulses / second)
		OCR1A = 12499;						//Interrupt every millisecond (12500 pulses / ms)
		TIMSK1 |= (1<<OCIE1A);				//Enable interrupt	
		return;
	}
	#endif
}


int get_time(struct timeval* time){
	time->h = _time_h;
	time->m = _time_m;
	time->s = _time_s;
	return 0;
}


/*
 *	Print time to the buffer. Format HH:MM:SS
 */
int time_print(char *buf){
	return sprintf_P(buf, PSTR("%02u:%02u:%02u"), _time_h, _time_m, _time_s);
}


/*
 *
 */
int get_clock(struct clockval* clock){
	return 0;
}


/*
 *	Print clock to the buffer. Format HH:MM:SS
 */
int clock_print(char *buf){
	return sprintf_P(buf, PSTR("%02u:%02u:%02u"), _clock_h, _clock_m, _clock_s);
}

/*
 *
 */
void timer_start(struct clockval* timer){
	timer->h = _clock_h;
	timer->m = _clock_m;
	timer->s = _clock_s;
	timer->ms = _clock_ms;
}


/*
 *
 */
void timer_stop(struct clockval* timer){
	timer->h = _clock_h - timer->h;
	timer->m = _clock_m - timer->m;
	timer->s = _clock_s - timer->s;
	timer->ms = _clock_ms - timer->ms;
}


/*
 *
 */
uint32_t get_elapsed_time(struct clockval* timer){
	
	uint32_t time = (_clock_h - timer->h) * 3600000;	//Hour to ms
	time += (_clock_m - timer->m) * 60000;	//Minute to ms
	time += (_clock_s - timer->s) * 1000;	//Seconds to ms
	time += _clock_ms - timer->ms;
	return time;
}


/*
 *
 */
ISR(TIMER1_COMPA_vect){
	
	//static uint16_t lcd_timer = (LCD_REFRESH - 1);
	
	//Following executed every millisecond
	//if(!lcd_timer--){
	//	lcd_timer_flag = 1;
	//	lcd_timer = (LCD_REFRESH - 1);		
	//}
	
	if(_clock_ms++ == 999){	//Following executed every second
		_clock_ms = 0;
		
		//tmp_timer_flag = 1;		//Set temperature update flag every second
		
		if(_clock_s++ == 59){	//Following executed every minute
			_clock_s = 0;
			
			
			if(_clock_m++ == 59){	//Following executed every hour
				_clock_m = 0;
				_clock_h++;
			}
		}
		
		if(_time_s++ == 59){
			_time_s = 0;
			
			
			if(_time_m++ == 59){
				_time_m = 0;
				
				if(_time_h++ == 23){
					_time_h = 0;
				}
			}
		}
	}
}
