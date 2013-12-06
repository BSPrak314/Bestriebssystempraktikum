
#include <printf.h>
#include <sys_timer.h>
#include <utils.h>
#include <scanf.h>
#include <buffer.h>
#include <led.h>
#include <dbgu.h>
#include <systemtests.h>
#include <console.h>
#include <regcheck.h>
#include <exception_handler.h>


void console ( void )
{
	char* help = "help";
	char* boot = "reboot";
	char* provoke = "provoke";
	char* led = "led";
	char* printReg = "print";
	char* regCheck = "regchecker";
	char* currenttest = "current test";
	
	char enter = '\n';
	char *info = "implemented commands:\n -> led <red|green|yellow> <on|off>\n -> reboot\n -> print <cpsr|registers>\n -> provoke <abort|software|undefined>\n -> currenttest\n -> regchecker\n> \n";
	char buf[BUFFERSIZE];
	printf("> type help to show all implemented commands\n");
	printf("> type regchecker to start register_checker routine\n");
	printf("> type current test to start testroutine for the 3th excersise\n");
	printf("> type exit during current test to end the test\n> \n> ");
	
	int i = 0;
	
	while(1){

		char c = 0;
	    
	    if( dbgu_hasBufferedInput() ){
        	c = dbgu_nextInputChar();
        	buf[i] = c;
        }

		if(i >= BUFFERSIZE)
			i = 0;
	      
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
			}else if( startsWith(buf, currenttest) ){
				testBufferedIO();
			}
			
			i = 0;
			printf("> ");
		
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

void reboot( void )
{
	st_setWatchdogValue(0x00000001);
	st_enableWDT();
}

void printInfo( char* string )
{

	char* cpsr = "cpsr";
	char* registers = "registers";
	//char* regnr = "reg nr";

	string = string +6;
	if( startsWith(string, cpsr) ){
		print_cpsr();
		return;
	}else if( startsWith(string, registers) ){
		print_allRegisters();
		return;
	}
	/*
	else if( startsWith(string, regnr) ){
		asm_printRegisters();
		return;
	}*/
	printf("> can not recognize print attributes\n");
	printf("> type print <cpsr|registers|>\n>");
	return;
}

void usingLeds( char* string )
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
	printf("> type led <red|green|yellow> <on|off>\n>");
	return;
}	

void provokeInterrupt( char* string )
{
	char* abort = "abort";
	char* software = "software";
	char* undefined = "undefined";
	
	string = string +8;
	if( startsWith(string, abort) ){
		provoke_data_abort();
	}else if( startsWith(string, software) ){
		provoke_sw_inter();
	}else if( startsWith(string, undefined) ){
		provoke_undef_inst();
	}else{
		printf("> can not recognize interrupt type\n");
		printf("> type provoke <abort|software|undefined> to provoke interrupts\n>");
	}
}