
#include <stdarg.h>
#include <dbgu.h>
#include <printf.h>
#include <reg_operations_asm.h>

static void printBinaer(unsigned int address)
{
        printf("0b");
        int i = 31;

        for(;i<=0;i--){
                if( address & ( 1 << i ) )
                        dbgu_bufferedOutput('1');
                else
                        dbgu_bufferedOutput('0');
        }
}

static void printHex(unsigned int address, int do_prefix)
{
        char hexASCII[] = "0123456789ABCDEF";
        unsigned int mask = 0xf0000000;
        
        if(do_prefix){
                printf("0x");
        }
        // look at the 4 MSB bits
        unsigned int c = (address & mask);
        // bring those bits to the range 0 to 15
        c = c >> 28;
        
        int i = 0;
        for(;i<8;){
                dbgu_bufferedOutput(hexASCII[c]);
                // adjusting mask and updating the next 4 bits to look at
                i++;
                mask = mask >> 4;
                c = (address & mask);
                c = c >> (28 - i*4);
        }
}

/* testing the attribute list for correct datatyps */
//__attribute__( ( format(printf,1,2) ) )
// there is no attribute for binar attributes - so i prefer no typechecking
void printf( char *str, ... )
{
        // makros to deal with variable arguments, imported from stdargs.c
        va_list arglist;
        va_start(arglist, str);
        char c = *str;
        str++;
        while(c != 0){          
                char d = *str;
                str++;
                if(c == '%'){
                        switch(d){
                        // %c calls for an char argument for our ap-list
                        case 'c':{
                                int ch = va_arg(arglist, int);
                                dbgu_bufferedOutput((char)ch);
                                d = *str;
                                str++;
                                break;
                                }
                        // %s calls for an c-string argument from our ap-list
                        case 's':{
                                char *s = va_arg(arglist, char* );
                                printf(s);
                                d = *str;
                                str++;
                                break;
                                }
                        // an integer from our ap-list need to be displayed as hex-value
                        case 'x':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                printHex(i,1);
                                d = *str;
                                str++;
                                break;
                                }
                        // an integer from our ap-list need to be displayed as hex-value
                        case 'b':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                printBinaer(i);
                                d = *str;
                                str++;
                                break;
                                }
                        // an adresss form our ap-list need to be displayed
                        case 'p':{
                                void *addr = va_arg(arglist, void* );
                                printHex( ((unsigned int)addr) ,0 );
                                d = *str;
                                str++;
                                break;
                                }
                        // its just an % char
                        default:
                                dbgu_bufferedOutput(c);
                        }
                // no control-sequence
                } else {
                        dbgu_bufferedOutput(c);
                }
                c = d;
        }
        va_end(arglist);
}

static void printH(unsigned int address, int do_prefix)
{
        char hexASCII[] = "0123456789ABCDEF";
        unsigned int mask = 0xf0000000;
        
        if(do_prefix){
                print("0x");
        }
        
        // look at the 4 MSB bits
        unsigned int c = (address & mask);
        
        // bring those bits to the range 0 to 15
        c = c >> 28;
        
        int i = 0;
        for(;i<8;){

                dbgu_writeChar( hexASCII[c] );
                
                // adjusting mask and updating the next 4 bits to look at
                i++;
                mask = mask >> 4;
                c = (address & mask);
                c = c >> (28 - i*4);
        }
}

/* testing the attribute list for correct datatyps */
__attribute__( ( format(printf,1,2) ) )
void print( char *str, ... )
{
        // makros to deal with variable arguments, imported from stdargs.c 
        va_list arglist;
        va_start(arglist, str);
          
        char c = *str;
        str++;
        
        while(c != 0){
                
                char d = *str;
                str++;
                        
                if(c == '%'){

                        switch(d){
                        // %c calls for an char argument for our ap-list
                        case 'c':{
                                int ch = va_arg(arglist, int);
                                dbgu_writeChar((char)ch);
                                d = *str;
                                str++;
                                break;
                                }
                        // %s calls for an c-string argument from our ap-list
                        case 's':{
                                char *s = va_arg(arglist, char* );
                                print(s);
                                d = *str;
                                str++;
                                break;
                                }
                        // an integer from our ap-list need to be displayed as hex-value
                        case 'x':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                printH(i,1);
                                d = *str;
                                str++;
                                break;
                                }
                        // an adresss form our ap-list need to be displayed
                        case 'p':{
                                void *addr = va_arg(arglist, void* );
                                printH( ((unsigned int)addr) ,0 );
                                d = *str;
                                str++;
                                break;
                                }
                        // its just an % char
                        default:
                                dbgu_writeChar(c);
                        }
                // no control-sequence
                } else {
                        dbgu_writeChar(c);
                }
                c = d;
        }
        va_end(arglist);
}

//Prints CPSR in binaer coding, buffered printing, so output will display when interrupt handling is done
void print_cpsr( void )
{
        unsigned int cpsr = asm_getCPSR();
        print("cpsr : [<%x>]\n> \n",cpsr);
}

//Prints register nr reg, buffered printing, so output will display when interrupt handling is done
void print_register( unsigned int reg )
{       
        if( reg > 12 && reg != 14 ){
                print("print register can only print register r0 to r12 and r14 := lr \nPlease retry with a register number between 0 and 12 or 14\n >\n");
                return;
        }
        struct reg_info *registers = (struct reg_info *)asm_getRegisters();
        
        unsigned int reg_contains = registers->lr;
        if(reg < 13)
                reg_contains = registers->r[reg];
        
        print("reg_%x : [<%x>]\n", reg,reg_contains);
}

//Prints all registers, buffered printing, so output will display when interrupt handling is done
void print_reginfo( struct reg_info * reg)
{
        print("printing registers...\n");
        print("         lr : [<%x>]\n", reg->lr);
        print("r0 : %x  r1 : %x  r2 : %x\n",reg->r[0], reg->r[1], reg->r[2]);
        print("r3 : %x  r4 : %x  r5 : %x\n",reg->r[3], reg->r[4], reg->r[5]);
        print("r6 : %x  r7 : %x  r8 : %x\n",reg->r[6], reg->r[7], reg->r[8]);
        print("r9: %x  r10: %x  r11 : %x\n",reg->r[9], reg->r[10], reg->r[11]);
        print("> \n");
}

//Prints all registers, printing via polling, so output will display during interrupt handling
void print_RegisterStruct( struct registerStruct * reg)
{
        print("         lr_irq    : [<%x>]\n", reg->pc);
        unsigned int mode = reg->cpsr & 0x0000001F;
        if( mode == 0x1F ){
                print("         sp_sys   : [<%x>]\n", reg->sp);
                print("         lr_sys   : [<%x>]\n", reg->lr);
                print("         cpsr_sys : [<%x>]\n", reg->cpsr);
        }else{
                print("         sp_user   : [<%x>]\n", reg->sp);
                print("         lr_user   : [<%x>]\n", reg->lr);
                print("         cpsr_user : [<%x>]\n", reg->cpsr);  
        }
        print("  r0 : %x  r1 : %x  r2 : %x\n",reg->r[0], reg->r[1], reg->r[2]);
        print("  r3 : %x  r4 : %x  r5 : %x\n",reg->r[3], reg->r[4], reg->r[5]);
        print("  r6 : %x  r7 : %x  r8 : %x\n",reg->r[6], reg->r[7], reg->r[8]);
        print("  r9: %x  r10: %x  r11 : %x\n",reg->r[9], reg->r[10], reg->r[11]);
}

void print_Thread( struct thread * thread)
{
        print("Thread   #%x \n",thread->pos);
        print("   status %x \n",thread->status);
        print("      id  %x \n",thread->id);
        print("    stamp %x \n",thread->timestamp);
        print(" next pos %x \n",((struct thread *)&(thread->connect.next))->pos );
        print(" prev pos %x \n",((struct thread *)&(thread->connect.prev))->pos );
        print_RegisterStruct(&thread->regs);
}

void print_Threadlist(struct list * list)
{
        struct list * tmp = list_getHead(list);
        print("printThreadList:\n");
        while( tmp ){
                struct thread * currThread = (struct thread *)tmp;
                print_Thread(currThread);
                print("---\n");
                tmp = list_getHead( &currThread->connect );
        
        }
        print("  printThreadList done\n");
}