
#include <dbgu.h>
#include <printf.h>
#include <utils.h>
#include <scanf.h>
#include <led.h>
#include <regcheck.h>

extern char* infoPIT;

// Test functions for implemented functions
void provoke_data_abort(void) // Provoked data abort by writing to bad address
{
        unsigned int *bad_address = (unsigned int *)0x92000000;
        unsigned int dataabort = 12345;
        *bad_address = dataabort;
        if(bad_address)
                return;
}
   
void provoke_sw_inter(void)
{
        asm("swi #0x00":::);
}

void provoke_undef_inst(void)
{
        asm(".word 0xa000f7f0");
}

void testBufferedIO(void)
{
        infoPIT = "!\n";
        int pos = 0;
        char buf[4];
        char c = 0;
        while(1){
                if( dbgu_hasBufferedInput() ){
                        c = dbgu_nextInputChar();
                        buf[pos] = c;
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
                        if( startsWith(buf, "exit") ){
                                infoPIT = 0;            
                                return;
                        }
                }
                waitBusy(20);
        }
}