
#ifndef _thread_h_
#define _thread_h_

#include <list.h>

#define THEAD_CTRL_BASE 	0x00200040
#define THEAD_ARRAY_BASE 	(THEAD_CTRL_BASE + SIZE_OF_THREADCTRL)

#define USERSTACK_BASE  	0x23E00000
// Stacksize: 4 kByte
#define THREAD_STACKSIZE (4 * 1024)

#define SWI_WRITE 	0x0FF000
#define SWI_READ 	0x0F0000
#define SWI_KILL 	0xFFFFFF
#define SWI_CREATE 	0x00000F
#define SWI_WAIT 	0x0000FF
#define SWI_YIELD 	0x000FFF

#define DEAD 0
#define ACTIVE 1
#define RUNNING 2
#define SLEEPING 3
#define WAIT_INPUT 4

#define MAX_THREADS 16

#define NR_OF_REGS 13
#define SIZE_OF_REGISTER_STRUCT ( (NR_OF_REGS+4)*4 )
#define SIZE_OF_THREAD 		( 6*4 +SIZE_OF_LIST +SIZE_OF_REGISTER_STRUCT )
#define SIZE_OF_THREADCTRL 	( 2*4 + 3*SIZE_OF_LIST )

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
	struct registerStruct regs;
};

struct threadArray{
	struct thread threads[MAX_THREADS];
	struct thread idleThread;
};

struct threadQueue{
	struct thread *running;
	struct list emptyList;
	struct list activeList;
	struct list sleepingList;
	struct thread *curr_IO;
};

int 	thread_initThreadControl( void );
int 	thread_runSheduler( struct registerStruct * );
int 	thread_create( void * function, void * params, struct registerStruct * );
int 	thread_dealWithSWI( unsigned int swiCode, struct registerStruct * regStruct );
int 	thread_wakeUp( struct registerStruct * regStruct );
int 	thread_infoAboutInput( struct registerStruct * regStruct );
int 	thread_kill( struct registerStruct * regStruct );
unsigned int thread_getRunningID( void );

#endif