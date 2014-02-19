#ifndef _syscall_asm_H_
#define _syscall_asm_H_

int  	asm_syscall_write( char );
int 	asm_syscall_read( void );
int 	asm_syscall_kill( void );
int	asm_syscall_fork( void *function, void *params );
int	asm_syscall_newProcess( void *function, void *params );
int 	asm_syscall_yield( void );
int 	asm_syscall_wait( unsigned int );
int 	asm_endless_loop( void );
int 	asm_syscall_get_memory( void );
unsigned int 	asm_syscall_get_id( void );
unsigned int 	asm_syscall_get_threadid( void );
unsigned int 	asm_syscall_get_localid( void );

#endif