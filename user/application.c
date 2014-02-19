#include <syscall.h>
#include <syscall_asm.h>
#include <application.h>
#include <user_lib.h>
// not using utils function startsWith() or strCompare()
// because lib_user should be independent

#define iter 0x10

void app_thread_app7()
{
        volatile int id = (syscall_get_id() >> 24);
 
        volatile int local = syscall_get_localid();
 
        volatile int j = 0;
 
	volatile unsigned char c = (char)(*(unsigned int *)0x00000000);
        //user_print("                 %p %c start\n",local, c);
        while(1){
                volatile unsigned int i = *(unsigned int *)0x00000004;
                
                if( i > iter){
                        //user_print("                            %p %c done\n",local,c);
                        break;
                }
                c = (char)(*(unsigned int *)0x00000000);
         
                user_print("%c%p [%p]: %p(%p)\n",c,local,id,i,j);
                j++;
                i++;
         
                *(unsigned int *)0x00000004 = i;
         
                syscall_wait(800);
         
        }       
        syscall_exit();
}

void app_process_app7( char c )
{
        *(unsigned int *)0x00000004 = 0;
        *(unsigned int *)0x00000000 = (int)c;
        
        volatile int k = 0;
        for(;k<2;k++){
                syscall_fork( (void *)&app_thread_app7, (void *)0 );
        }
	
        app_thread_app7();
}

void app_run_app7( void )
{
        user_print("\n****************************\n");
        user_print("*       Aufgabe 7          *\n");
        user_print("****************************\n\n");
 
        volatile char c = 0;
        
        while(1){
                c = syscall_readChar();
                syscall_newProcess( (void *)&app_process_app7, (void *)(int)c );
        }
        user_print("Aufgabe 7 done\n");
        syscall_exit();
}