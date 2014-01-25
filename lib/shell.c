
#include <printf.h>
#include <sys_timer.h>
#include <utils.h>
#include <buffer.h>
#include <led.h>
#include <dbgu.h>
#include <systemtests.h>
#include <regcheck.h>
#include <interrupt_handler.h>

#include <list.h>

#define SHELLBUF_SIZE 80

void reboot( void )
{
	st_setWatchdogValue(0x00000001);
	st_enableWatchdogReset();
	st_enableWDT();
}

static void cleanDisplay( void )
{
	int i = 0;
	for(i = 0;i<40;i++)
		printf("\n");
	printf("> ");
}

static void printInfo( char* string )
{

	char* cpsr = "cpsr";
	char* registers = "registers";
	//char* regnr = "reg nr";

	string = string +6;
	if( startsWith(string, cpsr) ){
		print_cpsr();
		return;
	}else if( startsWith(string, registers) ){
		//print_allRegisters();
		printf("currently no implemented - sorry\n> ");
		return;
	}
	/*
	else if( startsWith(string, regnr) ){
		asm_printRegisters();
		return;
	}*/
	printf("> can not recognize print attributes\n");
	printf("> type print <cpsr|registers|list>\n> ");
	return;
}

static void doTests( char* string )
{
	char* bufferedIO = "bufferedIO";
	char* threads = "threads";
	char* list = "list";

	string = string +5;
	if( startsWith(string, bufferedIO) ){
		systest_testBufferedIO();
		return;
	}else if( startsWith(string, threads) ){
		systest_threadTest();
		return;
	}
	else if( startsWith(string, list) ){
		list_testEmbeddedListStruct();
		return;
	}
	printf("> can not recognize test to run\n");
	printf("> type test <bufferedIO|threads|list>\n> ");
	return;
}

static void provokeInterrupt( char* string )
{
	char* abort = "abort";
	char* software = "software";
	char* undefined = "undefined";
	
	string = string +8;
	if( startsWith(string, abort) ){
		systest_provoke_data_abort();
	}else if( startsWith(string, software) ){
		systest_provoke_sw_inter();
	}else if( startsWith(string, undefined) ){
		systest_provoke_undef_inst();
	}else{
		printf("> can not recognize interrupt type\n");
		printf("> type provoke <abort|software|undefined> to provoke interrupts\n> ");
	}
}

static void usingLeds( char* string )
{
  
	char* red = "red";
	char* green = "green";
	char* yellow = "yellow";
	char* on = "on";
	char* off = "off";
	
	string = string +4;
	if( startsWith(string, red) ){
		string = string +4;
		if( startsWith(string, on) ){
			red_on();
			return;
		}else if( startsWith(string, off) ){
			red_off();
			return;
		}
	}else if( startsWith(string, yellow) ){
		string = string +7;
		if( startsWith(string, on) ){
			yellow_on();
			return;
		}else if( startsWith(string, off) ){
			yellow_off();
			return;
		}
			
	}else if( startsWith(string, green) ){
		string = string +6;
		if( startsWith(string, on) ){
			green_on();
			return;
		}else if( startsWith(string, off) ){
			green_off();
			return;
		}
	}
	printf("> can not recognize led attributes\n");
	printf("> type led <red|green|yellow> <on|off>\n> ");
	return;
}

void shell_start( void )
{
	char* help = "help";
	char* boot = "reboot";
	char* provoke = "provoke";
	char* led = "led";
	char* printReg = "print";
	char* regCheck = "regchecker";
	char* currenttest = "current test";
	char* test = "test";
	char* clean = "clean";
	
	char *info = "implemented commands:\n -> led <red|green|yellow> <on|off>\n -> reboot\n -> print <cpsr|registers>\n -> provoke <abort|software|undefined>\n -> test <bufferedIO|threads|list>\n -> currenttest\n -> regchecker\n -> clean\n> \n";

	char enter = '\n';

	char buf[SHELLBUF_SIZE];
	printf("> type help to show all implemented commands\n");
	printf("> type regchecker to start register_checker routine\n");
	printf("> type current test to start testroutine for the 5th excersise\n");
	printf("> type exit during current test to end the test\n> \n> ");
	
	int i = 0;
	
	while(1){

		char c = 0;
	    	
	    	if(i >= SHELLBUF_SIZE)
			i = 0;

	    	if( dbgu_hasBufferedInput() ){
        		c = dbgu_nextInputChar();
        		buf[i] = c;
        	}
	      
		if(c == enter){
			dbgu_bufferedOutput(c);
			if( startsWith(buf, help) ){
				printf(info);
			}else if( startsWith(buf, boot) ){
				reboot();
			}else if( startsWith(buf, regCheck) ){
				register_checker();
			}else if( startsWith(buf, printReg) ){
				printInfo(buf);
			}else if( startsWith(buf, provoke) ){
				provokeInterrupt(buf);
			}else if( startsWith(buf, led) ){
				usingLeds(buf);
			}else if( startsWith(buf, test) ){
				doTests(buf);
			}else if( startsWith(buf, currenttest) ){
				systest_threadTest();
			}else if( startsWith(buf, clean) ){
				cleanDisplay();
			}
			i = 0;
			printf("\n> ");
		
		}else if(0x7F == (int)c){
			dbgu_bufferedOutput('\b');
			dbgu_bufferedOutput(' ');
			dbgu_bufferedOutput('\b');
			i--;	
		}else if( c != 0){
			dbgu_bufferedOutput(c);
			buf[i] = c;
			i++;
		}
	}
}