/*
 * time.h
 *
 * Created: 10.7.2011 18:54:49
 *  Author: Lauri L‰‰peri
 *
 *	Time.h provides time, clock and timer functions.
 *	AVR TIMER1 register A is used.
 *
 */ 


#ifndef TIME_H
#define TIME_H

//Compiler modifications for tests
#ifdef TEST	//Compiling for test
	#define ISR void interrupt_handler
	#define TIMER1_COMPA_vect void
#else	//Compiling for AVR
	#include <avr/interrupt.h>
#endif

#include <stdint.h>

//Time struct is used for storing time of day
struct timeval{
	uint8_t h;	//Hours
	uint8_t m;	//Minutes
	uint8_t s;	//Seconds
};

//Clock struct is used to store clock value where hours can be over 24
struct clockval{
	uint16_t h;
	uint8_t m;
	uint8_t s;
	uint16_t ms;
};


//Clock variables
volatile uint16_t _clock_h;
volatile uint8_t _clock_m;
volatile uint8_t _clock_s;
volatile uint16_t _clock_ms;

//Variables for time of day
volatile uint8_t _time_h;
volatile uint8_t _time_m;
volatile uint8_t _time_s;

//Timer
//struct clockval _timer_start;
//struct clockval _timer_end;


/*
 * Initializes interrups vector for the timer.
 */
void time_init(uint32_t f_cpu);

//Time functions
int time_set(const struct timeval* time);
int time_get(struct timeval* time);
int time_print(char* buf);

//Clock functions
int clock_set(const struct clockval* clock);
int clock_get(struct clockval* clock);
int clock_print(char* buf);

//Timer functions
/*
 *	Starts the timer. Timer variable is initialized
 *	with current clock value.
 */
void timer_start(struct clockval* timer);

/*
 *	Ends the timer. Elapsed time is saved to the timer variable. 
 *	The parameter struct must be first initialized with timer_start function.
 */
void timer_stop(struct clockval* timer);

/*
 *	Returns elapsed time from the initialized timer struct and current clock value in milliseconds.
 *	Overflows in 49 days which is not checked.
 */
uint32_t timer_get_elapsed(const struct clockval* timer);


/*
 *	Clock interrupt handler
 */
ISR(TIMER1_COMPA_vect);

#endif /* TIME_H */
