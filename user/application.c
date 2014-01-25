#include <syscall.h>
#include <syscall_asm.h>
#include <application.h>
// not using utils function startsWith() or strCompare()
// because lib_user should be independent
static int app_exit ( char* buf){
        return( buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't');
}

static void writeBusy( char c, unsigned int loops )
{
        int i = 0;
        for(i = 0;i<23;i++){
                syscall_writeChar((int)c);
                unsigned int j = loops;
                for(; j > 0; j--)
                        asm("" ::: "memory");
        }
}

static void writeWaiting( char c, unsigned int wait )
{
        int i = 0;
        for(i = 0;i<23;i++){
                syscall_writeChar((int)c);
                syscall_thread_wait(wait);
        }
}


static void user_print(char *s)
{
        while(*s){
                char c = *s++;
                syscall_writeChar((int)c);
        }
}

void app_thread_app5( char c )
{
        unsigned int wait = 800;

        unsigned int loops = 45000*wait;
        if( (int)c > 64 && (int)c < 91 ){
                writeBusy(c,loops);
        }else{
                writeWaiting(c,wait);
        }
        syscall_thread_exit();
}

void app_run_app5( void )
{
        char exitBuffer[4];
        unsigned int c = 0;
        int i = 0;

        while(1){
                c = syscall_readChar();
                if( c != 0 ){
                        exitBuffer[i] = (char)c;
                        i++;
                        if(i == 4)
                                i = 0;
                        syscall_thread_create( (void *)&app_thread_app5, (void *)&c);
                        
                }
                if( app_exit(exitBuffer) )
                        break;
	}
        syscall_thread_exit();
}

void endless_recursion(void)
{
        asm_endless_loop();
}

void app_thread_app6( unsigned int nr )
{

        volatile unsigned int tmp = 0;

        switch(nr){
        case 0 :
                user_print("TEST 1:\n");
                user_print("lesender Zugriff auf Nullpointer / Addresse 0x00000000:\n\n");
                tmp = *(unsigned int *)0x00000000;
                break;
                
        case 1 :
                user_print("TEST 2:\n");
                user_print("lesender Zugriff auf Kernel Daten / Addresse 0x20000020:\n\n");
                tmp = *(unsigned int *)0x20000020;
                break;
                
        case 2 :
                user_print("TEST 3:\n");
                user_print("schreibender Zugriff auf eigenen Code / Addresse 0x20100020 :\n\n");
                *(unsigned int *)0x20100020 = tmp;
                break;
                
        case 3 :
                user_print("TEST 4:\n");
                user_print("Stack-Overflow durch endlose Rekursion:\n\n");
                endless_recursion();
                break;
                
        case 4 :
                user_print("TEST 5:\n");
                user_print("lesender Zugriff auf Addresse 0x90001000:\n\n");
                tmp = *(unsigned int *)0x90001000;
                break;
                
        case 5 :
                user_print("TEST 6:\n");
                user_print("Demonstration eines nicht 1:1 Mappings\n");
                user_print("Es werden High Exception verwendet...\n\n");
                user_print("Außerdem werden die Addresse 0x004X XXXX auf die Addressen 0x210X XXXX gemapped\n");
                user_print("und der UserMode hat hier Leseberechtigung - read at 0x00400008:\n\n");
                tmp = *(unsigned int *)0x00400008;
                user_print("was no problem\n\n");
                break;
                
        default : 
                ;
        
        }
        syscall_thread_exit();
}

void app_run_app6( void )
{

        user_print("Aufgabe 6\n");
        int i = 0;
        for(i = 0;i<6;i++){
                user_print("Beliebige Taste drücken um den nächsten Test zu starten\n");
                syscall_readChar();
                syscall_thread_create( (void *)&app_thread_app6, (void *)&i);
        }
        user_print("TESTS aus Aufgabe 6 done:\n");
        syscall_thread_exit();
}