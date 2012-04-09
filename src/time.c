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
	//Initialize AVR interrupt on timer 1
	TCCR1B = (1<<WGM12) | (1<<CS10);	//CTC mode, prescaling 1 (f_cpu timer pulses / second)
	if(f_cpu == 6250000){				//Clock is 6.25MHz
		OCR1A = 6249;					//Interrupt every millisecond (6250 pulses / ms)
	}else
	if(f_cpu == 12500000){				//Clock is 12.5MHz
		OCR1A = 12499;					//Interrupt every millisecond (12500 pulses / ms)
	}
	TIMSK1 |= (1<<OCIE1A);				//Enable interrupt	
	#endif
}


/*
 *
 */
int time_set(const struct timeval* time){
	
	//Check input validity
	if(time->h >= 24){
		return -1;
	}
	if(time->m >= 60){
		return -1;
	}
	if(time->s >= 60){
		return -1;
	}
	//Set valid input
	_time_h = time->h;
	_time_m = time->m;
	_time_s = time->s;
	return 0;
}


/*
 *
 */
int time_get(struct timeval* time){
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
int clock_set(const struct clockval* clock){
	
	//Check input validity (hour can be greater than 23)
	if(clock->m >= 60){
		return -1;
	}
	if(clock->s >= 60){
		return -1;
	}
	if(clock->ms >= 1000){
		return -1;
	}
	//Set valid input
	_clock_h = clock->h;
	_clock_m = clock->m;
	_clock_s = clock->s;
	_clock_ms = clock->ms;
	return 0;
}


/*
 *
 */
int clock_get(struct clockval* clock){
	clock->h = _clock_h;
	clock->m = _clock_m;
	clock->s = _clock_s;
	clock->ms = _clock_ms;
	return 0;
}


/*
 *	Print clock to the buffer. Format HHH:MM:SS.sss
 */
int clock_print(char *buf){
	return sprintf_P(buf, PSTR("%03u:%02u:%02u.%03u"), _clock_h, _clock_m, _clock_s, _clock_ms);
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
uint32_t timer_get_elapsed(const struct clockval* timer){
	
	//printf("%02u:%02u:%02u.%03u\n", _clock_h, _clock_m, _clock_s, _clock_ms);
	//printf("%02u:%02u:%02u.%03u\n", timer->h, timer->m, timer->s, timer->ms);
	
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
	
	if(_clock_ms++ >= 999){	//Following executed every second
		_clock_ms = 0;
		
		if(_clock_s++ >= 59){	//Following executed every minute
			_clock_s = 0;
			
			
			if(_clock_m++ >= 59){	//Following executed every hour
				_clock_m = 0;
				_clock_h++;
			}
		}
		
		//Time of day
		if(_time_s++ >= 59){
			_time_s = 0;
			
			
			if(_time_m++ >= 59){
				_time_m = 0;
				
				if(_time_h++ >= 23){
					_time_h = 0;
				}
			}
		}
	}
}
