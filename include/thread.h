
#ifndef _thread_h_
#define _thread_h_

#include <list.h>

#define THEAD_CTRL_BASE 0x00200040
#define THEAD_ARRAY_BASE 0x0020005c

#define SP_EXTERNAL_RAM 0x24000000
// Stacksize: 2 kByte
#define THREADSTACKSIZE (2 * 1024)		

#define DEAD 0
#define ACTIVE 1
#define RUNNING 2
#define SLEEPING 3

#define MAX_THREADS 32

#define NR_OF_REGS 13
#define SIZE_OF_REGISTER_STRUCT (17*4) 	  // = 68 byte
#define SIZE_OF_THREAD (17*4 + 2 + 5*4)   // = 90 byte
#define SIZE_OF_THREADCTRL (3*2*4 + 4) 	  // = 28 byte

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
	struct registerStruct regs;
	unsigned int inital_sp;
	unsigned int pos;
	unsigned int id;
	unsigned int timestamp;
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
};

int 	thread_enableThreads( void );
int 	thread_runSheduler( struct registerStruct * );
int 	thread_create( void * function, void * params, struct registerStruct * );
int 	thread_yield( void );
int 	thread_exit( void );
int 	thread_dealWithSWI(unsigned int swiCode, struct registerStruct * regStruct );

#endif