
#ifndef _thread_h_
#define _thread_h_

#define THREAD_ENABLED 1
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

#define TOTAL_THREADSIZE 32
#define IDLE_ENABLED 1
#define IDLE_DISABLED 0

#define NR_OF_REGS 13

struct thread{
	//struct list connect;
	unsigned int status;
	unsigned int reg[NR_OF_REGS];
	unsigned int lr;
	unsigned int sp;
	unsigned int cpsr;
	unsigned int priority;
	unsigned int timestamp;
	unsigned int sleeptime;
	unsigned int wakeUpCode;
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
	unsigned int nr_activeThreads;
	unsigned int nr_sleepingThreads;
};

void 	saveRegisterInArray( unsigned int );
int 	thread_enableThreads( void );
int 	thread_runSheduler( void );
int 	thread_switch( int );
int 	thread_start( void * );
void 	thread_close( void );
int 	thread_kill( void );
int 	thread_startIdle( void );
int 	thread_endIdle( void );
void 	thread_testContextChange( void );
void 	idle_thread( void );
void 	dummy_thread( void );
void 	thread_dealWithSWI( unsigned int, unsigned int );
void 	thread_wakeUp( void );

#endif