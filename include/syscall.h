
#ifndef _syscall_H_
#define _syscall_H_

int 	syscall_thread_yield( void );
int 	syscall_thread_wait( unsigned int msec );
int 	syscall_thread_exit( void );
int 	syscall_thread_create( void * function, void * params );
int 	syscall_readChar( void );
int 	syscall_writeChar( char );

#endif