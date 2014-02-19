
#include <syscall_asm.h>

/*
 * Convention for SYS-CALLS:
 * @return value: int value
 * return == 0 <> failure
 * return != 0 <> success
 *
 * @parameter
 * 3 unsigned int parameter
 *
 * all neccessary parsing will be done
 * -> in swi_call_asm for input params
 * -> in thread_dealWithSWI for output param
 * */

int syscall_readChar( void )
{
	return (int)asm_syscall_read();
}

int syscall_writeChar( char c )
{
	return asm_syscall_write( c );
}

/* enables a thread to wait for a periode
 * will perform an svc call */
int syscall_wait( unsigned int msec )
{
	if(msec == 0)
		return 0;
	return asm_syscall_wait(msec);
}

/* enables a thread to end himself 
 * will perform an svc call */
int syscall_exit( void )
{
	return asm_syscall_kill();
}

/* enables a thread to active give up his timeslice 
 * will perform an svc call */
int syscall_yield( void )
{
	return asm_syscall_yield();
}

/* enables a thread to start a new thread 
 * new thread will start immiately, taking the rest of the timeslice of the creating thread 
 * params: 	- void* functino := pointer the the entry function of the new thread
 * 		- void* params   := pointer to the parameter of the calling function */
int syscall_fork( void * function, void * params )
{
	return asm_syscall_fork(function, params);
}

/* enables a thread to start a new thread 
 * new thread will start immiately, taking the rest of the timeslice of the creating thread 
 * params: 	- void* functino := pointer the the entry function of the new thread
 * 		- void* params   := pointer to the parameter of the calling function */
int syscall_newProcess( void * function, void * params )
{
	return asm_syscall_newProcess(function, params);
}

int syscall_get_memory()
{
	return asm_syscall_get_memory();
}

unsigned int syscall_get_id()
{
	return asm_syscall_get_id();
}

unsigned int syscall_get_localid()
{
	return asm_syscall_get_localid();
}