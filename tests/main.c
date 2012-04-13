#include <embUnit/embUnit.h>

extern TestRef TimeTest_tests(void);
extern TestRef NetworkTest_tests(void);

int main (int argc, const char* argv[]){
	
	
	TestRunner_start();
	
	TestRunner_runTest(TimeTest_tests());
	TestRunner_runTest(NetworkTest_tests());
	
	TestRunner_end();
	return 0;
}  


