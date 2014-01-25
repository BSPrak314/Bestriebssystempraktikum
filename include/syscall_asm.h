#ifndef _syscall_asm_H_
#define _syscall_asm_H_

int  	asm_syscall_write( char );
int 	asm_syscall_read( void );
int 	asm_syscall_kill( void );
int	asm_syscall_create( void *function, void *params );
int 	asm_syscall_yield( void );
int 	asm_syscall_wait( unsigned int );
int 	asm_endless_loop( void );

#endif