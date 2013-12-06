
#ifndef _thread_h_
#define _thread_h_

#define THEAD_CTRL_BASE 0x00200040
#define THEAD_ARRAY_BASE 0x00200060

#define SP_INTERNAL_RAM 0x002FA000
#define SP_EXTRENAL_RAM 0x23FFA000
#define STACKSIZE 0x1000

#define ACTIVE 1
#define RUNNING 2
#define SLEEPING 3
#define DEAD 0

#define PRIORITY_HIGHEST = 4
#define PRIORITY_HIGH = 3
#define PRIORITY_NORMAL = 2
#define PRIORITY_LOW = 1

#define TOTAL_THREADSIZE 20
#define IDLE_ENABLED 1
#define IDLE_DISABLED 0

#define NR_OF_REGS 13

#define TIMESLICE 0x00000800

struct thread{
	//struct list connect;
	unsigned int status;
	unsigned int reg[NR_OF_REGS];
	unsigned int lr;
	unsigned int sp;
	unsigned int cpsr;
	unsigned int priority;
	unsigned int timestamp;
};

struct threadArray{
	struct thread threads[TOTAL_THREADSIZE];
};

struct thread_queue{

	struct thread *curr_running;
	unsigned int curr_pos;
	/*
	struct list empty;
	struct list active;
	struct list sleeping;
	*/
	unsigned int idle;
};

int thread_initQueue( void );
int thread_baseSheduler( void );
int thread_switch( unsigned int newActive );
struct thread * thread_newThread( void * function_pointer );
int thread_destroy( struct thread *deadThread );
void saveRegisterInArray( unsigned int entry );
//int thread_sleep( unsigned int thread_pos );
//int thread_wakeUp( unsigned int thread_pos );
int startIdle( void );
int endIdle( void );


#endif