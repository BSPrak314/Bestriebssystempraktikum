
#include <dbgu.h>
#include <interrupt_handler.h>
#include <printf.h>
#include <utils.h>
#include <regcheck.h>

/* message to print when PIT Interrupt detected, printed via buffered output, comes from sys_timer.c */
extern char* infoPIT;
/* message to print during context change, printed via buffered output, comes from thread.c */
extern char* infoContextChange; 

extern unsigned int thread_sheduler_enabled;

/* defined the handling of Software interrups, comes from interrupt_handler.c 
 * 1 = test enabled <=> swi interrupts print out register information and coded SWI instruction
 * 1 = test disabled <=> swi interrupts will delegated to thread.c and processed there */
extern unsigned int swi_test;

// Test functions for implemented functions
// Provoked data abort by writing to bad address
void systest_provoke_data_abort(void) 
{
        print("Systemtest: Data Abort due Undefined AdressSpace\n");
        unsigned int *bad_address = (unsigned int *)0x92000000;
        unsigned int dataabort = 12345;
        *bad_address = dataabort;
        
        print("Systemtest: Data Abort due Address Misalignment\n");
        unsigned int *bad = (unsigned int *)0x2C111111;
        unsigned int data = 0x2C111111;
        *bad = data;

        print("Test Data Abort Interrupt done\n >");
}
   
void systest_provoke_sw_inter(void)
{
        swi_test = 1;
        print("Systemtest: Software Interrupt Offset 0x0\n");
        asm("svc #0x0":::);
        print("Software Interrupt Offset 0x99\n");
        asm("svc #0x99":::);
        print("Software Interrupt Offset 0xFFFF\n");
        asm("svc #0xFFFF":::);
        print("Software Interrupt Offset 0x111111\n");
        asm("svc #0x111111":::);
        swi_test = 0;
        print("Test Software Interrupt done\n >");
}

void systest_provoke_undef_inst(void)
{
        print("Systemtest: Undefined Instruction\n");
        asm(".word 0xa000f7f0");
        print("Test Undefined Instruction Interrupt done\n >");
}

void systest_testBufferedIO(void)
{
        print("Systemtest: BufferedIO, controlled by interrupt not polling\n");
        int endTest = 0;
        infoPIT = "!\n";
        int pos = 0;
        char buf[4];
        char c = 0;
        while(1){
                if( dbgu_hasBufferedInput() ){
                        c = dbgu_nextInputChar();
                        buf[pos] = c;
                        if( startsWith(buf, "exit") ){
                                endTest = 1;
                        }
                        pos++;
                        if(pos == 4)
                                pos = 0;
                }
                if( c != 0 ){

                        int i = 0;
                        for(;i<12;i++){
                                waitBusy(800000);
                                print("%c",c);
                        }
                        c = 0;
                }
                if( endTest ){
                        infoPIT = 0;
                        print("Test BufferedIO done\n >");            
                        return;
                }
                waitBusy(20);
        }
}

void systest_dummyThread( void )
{
        char c = 0;
        while( c == 0){
                if( dbgu_hasBufferedInput() ){
                        c = dbgu_nextInputChar();
                }
        }
        int i = 0;
        for(i = 0;i<30;i++){
                print("%c",c);
                waitBusy(800000);
        }
}

void systest_threadTest( void )
{
        print("Systemtest: Thread Management  - Basictest with dummyThreads\n  Max. %x threads supported\nwhen there are no active threads\n -> switch to idle mode, disable PIT and turn off Systemclock\nwhen a  thread is done\n -> he calls a SWI to kill himself and call the sheduler\n\n",MAX_THREADS);
        /* make sure thread.c, thread_dealWithSWI deals with swi interrupts */
        swi_test = 0;
        /* each PIT interrupt now print ! and thread_switch now print newline */
        infoPIT = "!";
        infoContextChange = "\n";
        /* now in dbgu.c an TXRDY interrupt will create a new thread */
        thread_sheduler_enabled = 1;
}