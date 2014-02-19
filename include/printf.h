
#ifndef _printf_h_
#define _printf_h_

#include <thread.h>
#include <interrupt_handler.h>
//__attribute__( ( format(printf,1,2) ) )
// there is no attribute for binar attributes - so i prefer no typechecking
void printf( char *str, ... );

//__attribute__( ( format(printf,1,2) ) )
void print( char *str, ... );

void print_reginfo( struct reg_info *);
void print_RegisterStruct( struct registerStruct *);
void print_register( unsigned int );
void print_Threadlist(struct list * list);
void print_Thread( struct thread * thread);
void print_cpsr( void );

#endif