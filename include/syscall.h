
#ifndef _syscall_H_
#define _syscall_H_

int 	syscall_yield( void );
int 	syscall_wait( unsigned int msec );
int 	syscall_exit( void );
int 	syscall_fork( void * function, void * params );
int 	syscall_newProcess( void * function, void * params );
int 	syscall_readChar( void );
int 	syscall_writeChar( char );
int 	syscall_get_memory( void );
unsigned int 	syscall_get_id( void );
unsigned int 	syscall_get_localid( void );

#endif