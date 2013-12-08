
#include <dbgu.h>
#include <exception_handler.h>
#include <printf.h>
#include <utils.h>
#include <scanf.h>
#include <led.h>
#include <regcheck.h>

/* message to print when PIT Interrupt detected, printed via buffered output, comes from sys_timer.c */
extern char* infoPIT;
/* message to print during context change, printed via buffered output, comes from thread.c */
extern char* infoContextChange; 
/* enables/disables basic thread test in dbgu, comes from thread.c 
 * 1 = test enabled, 0 = test disabled */
extern unsigned int thread_test;

/* defined the handling of Software interrups, comes from exception_handler.c 
 * 1 = test enabled <=> swi interrupts print out register information and coded SWI instruction
 * 1 = test disabled <=> swi interrupts will delegated to thread.c and processed there */
extern unsigned int swi_test;

// Test functions for implemented functions
// Provoked data abort by writing to bad address
void systest_provoke_data_abort(void) 
{
        int testnr = 0;
        printf("Systemtest: Data Abort due Undefined AdressSpace\n");
        unsigned int *bad_address = (unsigned int *)0x92000000;
        unsigned int dataabort = 12345;
        *bad_address = dataabort;
        if(bad_address)
                testnr++;

        printf("Systemtest: Data Abort due Data Misalignment\n");
        unsigned int *bad_address2 = (unsigned int *)0x2C111111;
        unsigned int dataabort2 = 0x2C111111;
        *bad_address2 = dataabort2;

        if(bad_address2)
                testnr++;

        printf("Test Data Abort Interrupt done\n >");
}
   
void systest_provoke_sw_inter(void)
{
        swi_test = 1;
        printf("Systemtest: Software Interrupt Offset 0x0\n");
        asm("swi #0x0":::);
        printf("Software Interrupt Offset 0x99\n");
        asm("swi #0x99":::);
        printf("Software Interrupt Offset 0xFFFF\n");
        asm("swi #0xFFFF":::);
        printf("Software Interrupt Offset 0x111111\n");
        asm("swi #0x111111":::);
        swi_test = 0;
        printf("Test Software Interrupt done\n >");
}

void systest_provoke_undef_inst(void)
{
        printf("Systemtest: Undefined Instruction\n");
        asm(".word 0xa000f7f0");
        printf("Test Undefined Instruction Interrupt done\n >");
}

void systest_testBufferedIO(void)
{
        printf("Systemtest: BufferedIO, controlled by interrupt not polling\n");
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
                                printf("%c",c);
                        }
                        c = 0;
                }
                if( endTest ){
                        infoPIT = 0;
                        printf("Test BufferedIO done\n >");            
                        return;
                }
                waitBusy(20);
        }
}

void systest_threadTest( void )
{
        printf("Systemtest: Thread Management  - Basictest with dummyThreads\n");
        /* make sure thread.c, thread_dealWithSWI deals with swi interrupts */
        swi_test = 0;
        /* each PIT interrupt now print ! and thread_switch now print newline */
        infoPIT = "!";
        infoContextChange = "\n";
        /* now in dbgu.c an TXRDY interrupt will create a new thread */
        thread_test = 1;
}

