#include <syscall.h>
#include <stdarg.h>
#include <user_lib.h>

int app_exit ( char* buf)
{
        return( buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't');
}

static void user_printH(unsigned int address, int do_prefix)
{
        char hexASCII[] = "0123456789abcdef";
        unsigned int mask = 0xf0000000;
        
        if(do_prefix){
                user_print("0x");
        }
        
        // look at the 4 MSB bits
        unsigned int c = (address & mask);
        
        unsigned int start = 0;

        // bring those bits to the range 0 to 15
        c = c >> 28;
        
        int i = 0;
        for(;i<8;){

                if( c )
                        start = 1;
                if( start )
                        syscall_writeChar((int)( hexASCII[c]) );
                
                // adjusting mask and updating the next 4 bits to look at
                i++;
                mask = mask >> 4;
                c = (address & mask);
                c = c >> (28 - i*4);
        }
        if( !start )
                syscall_writeChar((int)( hexASCII[0]) );
}

/*
static void user_printD(unsigned int address)
{
        char decASCII[] = "0123456789";
        unsigned int mask = 0xf0000000;
        
        // look at the 4 MSB bits
        unsigned int c = (address & mask);
        
        unsigned int start = 0;

        // bring those bits to the range 0 to 15
        c = c >> 28;
        
        int i = 0;
        for(;i<8;){
                unsigned int overflow = 0;
                if( c )
                        start = 1;
                if( start ){
                        if( c < 10 )
                                syscall_writeChar((int)( decASCII[c]) );
                        else{
                                syscall_writeChar((int)( decASCII[c-10]) );
                                overflow = 1;
                        }
                }
                // adjusting mask and updating the next 4 bits to look at
                i++;
                mask = mask >> 4;
                c = (address & mask);
                c = c >> (28 - i*4);
                if( overflow )
                        c++;
        }
        if( !start )
                syscall_writeChar((int)( decASCII[0]) );
}
*/

/* testing the attribute list for correct datatyps */
//__attribute__( ( format(printf,1,2) ) )
void user_print( char *str, ... )
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
                                syscall_writeChar((int)ch);
                                d = *str;
                                str++;
                                break;
                                }
                        // %s calls for an c-string argument from our ap-list
                        case 's':{
                                char *s = va_arg(arglist, char* );
                                user_print(s);
                                d = *str;
                                str++;
                                break;
                                }
                        // an integer from our ap-list need to be displayed as hex-value
                        case 'x':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                user_printH(i,1);
                                d = *str;
                                str++;
                                break;
                                }
                        // an integer from our ap-list need to be displayed as hex-value
                        /*
                        case 'd':{
                                unsigned int i = va_arg(arglist, unsigned int );
                                user_printD(i);
                                d = *str;
                                str++;
                                break;
                                }
                        */
                        // an adresss form our ap-list need to be displayed
                        case 'p':{
                                void *addr = va_arg(arglist, void* );
                                user_printH( ((unsigned int)addr) ,0 );
                                d = *str;
                                str++;
                                break;
                                }
                        // its just an % char
                        default:
                                syscall_writeChar((int)c);
                        }
                // no control-sequence
                } else {
                        syscall_writeChar((int)c);
                }
                c = d;
        }
        va_end(arglist);
}