#include <embUnit/embUnit.h>                           
#include "../src/time.h"
#include <stdio.h>

static void setUp(void){
	//counterRef = Counter_counter();
}

static void tearDown(void){
	//Counter_dealloc(counterRef);
}

static void testInit(void){
	int i = 0;
	TEST_ASSERT_EQUAL_INT(0, i);
	
	struct timeval tval;
	get_time(&tval);
	char buf[32];
	time_print(buf);
	printf(buf);
	
}


TestRef TimeTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {                    
		new_TestFixture("testInit",testInit),
	};
	
	EMB_UNIT_TESTCALLER(TimeTest,"TimeTest",setUp,tearDown,fixtures);
	return (TestRef)&TimeTest;                        
}

