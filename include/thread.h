
#ifndef _thread_h_
#define _thread_h_

#include <list.h>

#define THEAD_CTRL_BASE 	0x20200000
#define SPACE_CTRL_BASE 	(THEAD_CTRL_BASE + SIZE_OF_THREADCTRL)
#define THEAD_ARRAY_BASE 	(THEAD_CTRL_BASE + SIZE_OF_THREADCTRL + SIZE_OF_SPACECTRL)

#define USERSTACK_BASE  	0x23E00000
#define VIRTUAL_STACK		0x00100000
// Stacksize: 4 kByte
#define THREAD_STACKSIZE 	(4 * 1024)

#define SWI_WRITE 		0x0FF000
#define SWI_READ 		0x0F0000
#define SWI_KILL 		0xFFFFFF
#define SWI_FORK 		0x00000F
#define SWI_NEWPROCESS 		0xF0000F
#define SWI_WAIT 		0x0000FF
#define SWI_YIELD 		0x000FFF
#define SWI_GET_MEMORY 		0x111111
#define SWI_GET_ID 		0x000001
#define SWI_GET_LOCALID		0x000002

#define DEAD 			0
#define ACTIVE 			1
#define RUNNING 		2
#define SLEEPING 		3
#define WAIT_INPUT 		4

#define MAX_THREADS 		64
#define MAX_ADD_SPACES		13

#define NR_OF_REGS 		13
#define NR_OF_EMPTYPAGES	40
#define SIZE_OF_REGISTER_STRUCT ( (NR_OF_REGS+4)*4 )
#define SIZE_OF_THREAD 		( 8*4 +SIZE_OF_LIST +SIZE_OF_REGISTER_STRUCT )
#define SIZE_OF_THREADCTRL 	( 2*4 + 3*SIZE_OF_LIST )
#define SIZE_OF_SPACE_STRUCT	( 3*4 )
#define SIZE_OF_SPACECTRL	( MAX_ADD_SPACES*SIZE_OF_SPACE_STRUCT + NR_OF_EMPTYPAGES*4)

struct registerStruct{
	unsigned int sp;
	unsigned int lr;
	unsigned int cpsr;
	unsigned int r[NR_OF_REGS];
	unsigned int pc;
};

struct thread{
	struct list connect;
	unsigned int status;
	unsigned int inital_sp;
	unsigned int pos;
	// optional could be extracted from inital_sp
	unsigned int id;
	unsigned int timestamp;
	unsigned int wakeTime;
	unsigned int address_Space;
	unsigned int local_id;
	struct registerStruct regs;
};

struct threadArray{
	struct thread threads[MAX_THREADS];
	struct thread idleThread;
};

struct add_Space{
	unsigned int domain;
	unsigned int running_threads;
	unsigned int pages;
};

struct add_Space_ctrl{
	struct add_Space spaces[MAX_ADD_SPACES];
	unsigned int emptyPages[NR_OF_EMPTYPAGES];
};

struct threadQueue{
	struct thread *running;
	struct list emptyList;
	struct list activeList;
	struct list sleepingList;
	struct thread *curr_IO;
};

int 	thread_initThreadControl( void );
int 	thread_runSheduler( struct registerStruct *, unsigned int alarm );
int 	thread_create( void * function, void * params, struct registerStruct * );
int 	thread_dealWithSWI( unsigned int swiCode, struct registerStruct * regStruct );
int 	thread_wakeUp( void );
int 	thread_infoAboutInput( struct registerStruct * regStruct );
int 	thread_kill( struct registerStruct * regStruct );
unsigned int thread_getRunningPosition( void );

#endif