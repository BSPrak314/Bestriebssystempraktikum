
#include <stdarg.h>
#include <dbgu.h>
#include <printf.h>

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
        
        /* look at the 4 MSB bits */
        unsigned int c = (address & mask);
        
        /* bring those bits to the range 0 to 15 */
        c = c >> 28;
        
        int i = 0;
        for(;i<8;){

                dbgu_bufferedOutput(hexASCII[c]);
                
                /* adjusting mask and updating the next 4 bits to look at*/
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
        /* makros to deal with variable arguments, imported from stdargs.c */
        va_list arglist;
        va_start(arglist, str);
        
        char c = *str;
        str++;
 
        while(c != 0){
                                
                char d = *str;
                str++;
                        
                if(c == '%'){

                        switch(d){
                        /* %c calls for an char argument for our ap-list */
                        case 'c':{
                                int ch = va_arg(arglist, int);
                                dbgu_bufferedOutput((char)ch);
                                d = *str;
                                str++;
                                break;
                                }
                        /* %s calls for an c-string argument from our ap-list */
                        case 's':{
                                char *s = va_arg(arglist, char* );
                                printf(s);
                                d = *str;
                                str++;
                                break;
                                }
                        /* an integer from our ap-list need to be displayed as hex-value*/
                        case 'x':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                printHex(i,1);
                                d = *str;
                                str++;
                                break;
                                }
                                 /* an integer from our ap-list need to be displayed as hex-value*/
                        case 'b':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                printBinaer(i);
                                d = *str;
                                str++;
                                break;
                                }
                        /* an adresss form our ap-list need to be displayed*/
                        case 'p':{
                                void *addr = va_arg(arglist, void* );
                                printHex( ((unsigned int)addr) ,0 );
                                d = *str;
                                str++;
                                break;
                                }
                        /* its just an % char */
                        default:
                                dbgu_bufferedOutput(c);
                        }
                /*
                 * no control-sequence
                 */
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
        
        /* look at the 4 MSB bits */
        unsigned int c = (address & mask);
        
        /* bring those bits to the range 0 to 15 */
        c = c >> 28;
        
        int i = 0;
        for(;i<8;){

                dbgu_writeChar( hexASCII[c] );
                
                /* adjusting mask and updating the next 4 bits to look at*/
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
        /* makros to deal with variable arguments, imported from stdargs.c */
        va_list arglist;
        va_start(arglist, str);
          
        char c = *str;
        str++;
        
        while(c != 0){
                
                char d = *str;
                str++;
                        
                if(c == '%'){

                        switch(d){
                        /* %c calls for an char argument for our ap-list */
                        case 'c':{
                                int ch = va_arg(arglist, int);
                                dbgu_writeChar((char)ch);
                                d = *str;
                                str++;
                                break;
                                }
                        /* %s calls for an c-string argument from our ap-list */
                        case 's':{
                                char *s = va_arg(arglist, char* );
                                print(s);
                                d = *str;
                                str++;
                                break;
                                }
                        /* an integer from our ap-list need to be displayed as hex-value*/
                        case 'x':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                printH(i,1);
                                d = *str;
                                str++;
                                break;
                                }
                        /* an adresss form our ap-list need to be displayed*/
                        case 'p':{
                                void *addr = va_arg(arglist, void* );
                                printH( ((unsigned int)addr) ,0 );
                                d = *str;
                                str++;
                                break;
                                }
                        /* its just an % char */
                        default:
                                dbgu_writeChar(c);
                        }
                /*
                 * no control-sequence
                 */
                } else {
                        dbgu_writeChar(c);
                }
                c = d;
        }
        va_end(arglist);
}