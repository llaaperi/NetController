#include <embUnit/embUnit.h>
#include <stdio.h>
#include "../src/time.h"

static void setUp(void){
}

static void tearDown(void){
}


/*
 *
 */
static void testInit(void){
	
	//Fill variables with some data
	_clock_h = 123;
	_clock_m = 123;
	_clock_s = 123;
	_clock_ms = 123;
	_time_h = 123;
	_time_m = 123;
	_time_s = 123;
	
	//Init time
	time_init(12500000);
	
	TEST_ASSERT_EQUAL_INT(0, _clock_h);
	TEST_ASSERT_EQUAL_INT(0, _clock_m);
	TEST_ASSERT_EQUAL_INT(0, _clock_s);
	TEST_ASSERT_EQUAL_INT(0, _clock_ms);
	
	TEST_ASSERT_EQUAL_INT(0, _time_h);
	TEST_ASSERT_EQUAL_INT(0, _time_m);
	TEST_ASSERT_EQUAL_INT(0, _time_s);
}


/*
 *
 */
static void testTimeSetAndGet(void){

	struct timeval tval;
	
	tval.h = 13;
	tval.m = 50;
	tval.s = 34;
	time_set(&tval);
	
	TEST_ASSERT_EQUAL_INT(13, _time_h);
	TEST_ASSERT_EQUAL_INT(50, _time_m);
	TEST_ASSERT_EQUAL_INT(34, _time_s);
	
	tval.h = 0;
	tval.m = 0;
	tval.s = 0;
	
	time_get(&tval);
	TEST_ASSERT_EQUAL_INT(13, tval.h);
	TEST_ASSERT_EQUAL_INT(50, tval.m);
	TEST_ASSERT_EQUAL_INT(34, tval.s);
}


/*
 *
 */
static void testClockSetAndGet(void){
	
	struct clockval cval;
	
	cval.h = 346;
	cval.m = 17;
	cval.s = 34;
	cval.ms = 456;
	clock_set(&cval);
	
	TEST_ASSERT_EQUAL_INT(346, _clock_h);
	TEST_ASSERT_EQUAL_INT(17, _clock_m);
	TEST_ASSERT_EQUAL_INT(34, _clock_s);
	TEST_ASSERT_EQUAL_INT(456, _clock_ms);
	
	cval.h = 0;
	cval.m = 0;
	cval.s = 0;
	cval.ms = 0;
	
	clock_get(&cval);
	TEST_ASSERT_EQUAL_INT(346, cval.h);
	TEST_ASSERT_EQUAL_INT(17, cval.m);
	TEST_ASSERT_EQUAL_INT(34, cval.s);
	TEST_ASSERT_EQUAL_INT(456, cval.ms);
}


/*
 *
 */
static void testClockInterrupt(void){
	
	time_init(1);
	
	int i;
	for(i = 0; i < 3661000; i++){
		interrupt_handler();	//Increment clock 3661000 times (1h 1m, 1s)
	}
	
	struct clockval cval;
	clock_get(&cval);
	TEST_ASSERT_EQUAL_INT(1, cval.h);
	TEST_ASSERT_EQUAL_INT(1, cval.m);
	TEST_ASSERT_EQUAL_INT(1, cval.s);
	TEST_ASSERT_EQUAL_INT(0, cval.ms);
	
	struct timeval tval;
	time_get(&tval);
	TEST_ASSERT_EQUAL_INT(1, tval.h);
	TEST_ASSERT_EQUAL_INT(1, tval.m);
	TEST_ASSERT_EQUAL_INT(1, tval.s);
}


/*
 *
 */
static void testTimer(void){
	
	time_init(1);
	struct clockval cval;
	timer_start(&cval);
	
	int i;
	for(i = 0; i < 500; i++){	//Increment timer 0.5s
		interrupt_handler();
	}
	uint32_t time = timer_get_elapsed(&cval);
	TEST_ASSERT_EQUAL_INT(500, time);
	
	for(i = 0; i < 600; i++){	//Increment timer 0.6s
		interrupt_handler();
	}
	time = timer_get_elapsed(&cval);
	TEST_ASSERT_EQUAL_INT(1100, time);
	
	for(i = 0; i < 80000; i++){	//Increment timer 1.2m
		interrupt_handler();
	}
	time = timer_get_elapsed(&cval);
	TEST_ASSERT_EQUAL_INT(81100, time);
	
	for(i = 0; i < 3800000; i++){	//Increment timer 1h 3m 20s
		interrupt_handler();
	}
	time = timer_get_elapsed(&cval);
	TEST_ASSERT_EQUAL_INT(3881100, time);
	
	timer_stop(&cval);	//3881100 ms = 1h 4m 41s 100ms
	TEST_ASSERT_EQUAL_INT(1, cval.h);
	TEST_ASSERT_EQUAL_INT(4, cval.m);
	TEST_ASSERT_EQUAL_INT(41, cval.s);
	TEST_ASSERT_EQUAL_INT(100, cval.ms);
}


TestRef TimeTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {                    
		new_TestFixture("testInit",testInit),
		new_TestFixture("testTimeSetAndGet",testTimeSetAndGet),
		new_TestFixture("testClockSetAndGet",testClockSetAndGet),
		new_TestFixture("testClockInterrupt",testClockInterrupt),
		new_TestFixture("testTimer",testTimer),
	};
	
	EMB_UNIT_TESTCALLER(TimeTest,"TimeTest",setUp,tearDown,fixtures);
	return (TestRef)&TimeTest;                        
}

