
#include <list.h>
#include <thread.h>
#include <dbgu.h>
#include <utils.h>
#include <printf.h>
#include <sys_timer.h>
#include <pmc.h>
#include <reg_operations_asm.h>
#include <interrupt_handler.h>
#include <swi_call_asm.h>


//#define TIMESLICE 0x00000000
#define TIMESLICE 0x00002000

/* flag to enable thread test with dummy threads, shared with dbgu */
unsigned int thread_test;
unsigned int thread_sheduler_enabled;

/* message to print during context change, printed via buffered output */
char* infoContextChange; 
extern char* infoPIT;

/* thread control structure */
static
struct threadQueue * const thread_ctrl = (struct threadQueue *)THEAD_CTRL_BASE;
/* thread array */
static
struct threadArray * const all = (struct threadArray *)THEAD_ARRAY_BASE;

/*
static void printThread( struct thread * thread)
{
	print("thread at pos %x\n",thread->pos);
	print("  has id      %x \n",thread->id);
	print("    timestamp %x \n",thread->timestamp);
	print("         next %x \n",(unsigned int)(thread->connect.next) );
	print("         prev %x \n",(unsigned int)(thread->connect.prev) );
	print("       status %x \n",thread->status);
}

static void listIterate(struct list * list)
{
	struct list * tmp = list_getHead(list);
	while( tmp ){
		struct thread * currThread = (struct thread *)tmp;
		printThread(currThread);
		tmp = list_getHead( &currThread->connect );
		
	}
}
*/

/* idle Mode - disable Processor Clock for IDLE MODE, every System interrupt enables Processor Clock again 
 *           - disable Periodic Intervall Interrupt, cause there are no running threads */
static void thread_startIdle( void )
{
	pmc_disableProcessorClock();
	st_disablePIT();
}

/* Idle Thread - calls startIdle in an endless loop */
static void thread_idleThread( void )
{
	while(1)
		thread_startIdle();
}

/* each threads get an individual id 
 * - first  8 bit for the position in our thread queue 
 * - last  24 bit for the timestamp when the thread was created */
static void thread_createID( struct thread * thread )
{
	thread->id = st_getTimeStamp();
	thread->id = (thread->id << 8);
	thread->id += thread->pos;
}

/* only thread_runSheduler, thread_create and thread_kill 
 * call for
 * thread_switch when - there are 2 or more active threads
 *                    - at the moment when threads get enabled, to switch to idle thread 
 *                    - the first active thread arrives to wake from idle
 *                    - the last active thread is dead, to switch to idle */
static int thread_switch( struct thread * newThread, struct registerStruct * regStruct )
{
	print(infoContextChange);

	// we have pointer to a running thread, avoid special case the first switch to idle
	if( thread_ctrl->running ){
		// running thread is not dead - save its register values, in coresponding thread struct
		if(thread_ctrl->running->status != DEAD){
			memcpy( &(thread_ctrl->running->regs), regStruct, SIZE_OF_REGISTER_STRUCT);
		}
		// now we put the former running thread in the right list
		if(thread_ctrl->running->status == SLEEPING){
			list_addTail( &(thread_ctrl->sleepingList),(struct list *)(thread_ctrl->running));	

		}else if(thread_ctrl->running->status == DEAD){
			list_addTail( &(thread_ctrl->emptyList),(struct list *)(thread_ctrl->running));

		}else if(thread_ctrl->running->status == RUNNING){
			// our idle thread has a defined position - he is sleeping now
			if( thread_ctrl->running->pos == MAX_THREADS ){
				thread_ctrl->running->status = SLEEPING;
			}else{
				thread_ctrl->running->status = ACTIVE;
				list_addTail( &(thread_ctrl->activeList),(struct list *)(thread_ctrl->running));
			}

		}
	}

	// we have a pointer to sp_irq,
	// all necessary registers for a context change is stored there in the same order as our registerStruct
	// perform the context change with memcopy - replace the context on the stack with new context
	memcpy(regStruct, &(newThread->regs), SIZE_OF_REGISTER_STRUCT);

	// update the information in our thread_ctrl struct
	thread_ctrl->running = newThread;
	thread_ctrl->running->status = RUNNING;
	thread_ctrl->running->timestamp = st_getTimeStamp();

	return 1;
}

/* only thread_exit calls this function via SWI-interrupt
 * this function performs a kill, be setting the status of the current thread to DEAD
 * next thread get his full timeslice, next thread will be determined by our sheduler */
static int thread_kill( struct registerStruct * regStruct )
{	
	thread_ctrl->running->status = DEAD;
	st_setPeriodicValue(TIMESLICE);

	return thread_runSheduler(regStruct);
}
/* thread_yield calls this funciton via SWI-interrupt
 * this function calls the sheduler - which will deal with the running thread 
 * when his is still active -> at the end of active queue
 *          is sleeping     -> at the end of the sleeping queue
 *          is dead         -> to his grave BUT will not kill the thread !!  */
static int thread_calledYield( struct registerStruct * regStruct )
{
	st_setPeriodicValue(TIMESLICE);
	return thread_runSheduler(regStruct);
}

static int thread_newThread( struct registerStruct * regStruct )
{

	unsigned int * pointsToThread = (unsigned int *)regStruct->r[0];
	struct thread * newThread = (struct thread *)pointsToThread;
	
	int out = thread_switch(newThread, regStruct );
	if(out){
		return newThread->id;
	}
	print("##################################################\nERROR DURING THREAD_newTHREAD - DUNRING THREAD_SWITCH\n");
	return 0;
}

/* init our thread control structure, by
 * - sets the base-address for stackpointer for all thread in queue
 * - sets status of all thread in our queue to DEAD <=> empty queue
 * - sets pointer to running thread to start of thread queue and curr_pos = 0
 * so when switching to our first thread, curr_running is DEAD and
 * thread_switch knows there are no register to save from previous thread 
 * - sets the PeriodicTimerIntervall to our TIMESLICE, so each PIT inducates a contextchange */
int thread_enableThreads( void )
{

	thread_test = 0;
	infoContextChange = "\0"; 
	infoPIT = "\0";
	thread_sheduler_enabled = 0;

	st_setPeriodicValue(TIMESLICE);
	
	// clear Memory 
	// begin at threadQueue struct, goes over to tmp_regs struct and over the whole threadArray struct
	unsigned int endValue = THEAD_CTRL_BASE + SIZE_OF_THREADCTRL + (MAX_THREADS+1) * SIZE_OF_THREAD;

	clearMemory( THEAD_CTRL_BASE, endValue);

	int i = 0;
	
	for(i = 0;i<MAX_THREADS;i++){
		// all threads starts dead <=> this array position is empty
		all->threads[i].status = DEAD;
		// inital stack pointer for this array position - will never change
		all->threads[i].inital_sp = SP_EXTERNAL_RAM - i*THREADSTACKSIZE;
		// the current stackspointer is the inital stack pointer for this thread position
		all->threads[i].regs.sp = all->threads[i].inital_sp;
		// position of this thread in threadArray
		all->threads[i].pos = i;
		// all threads to the empty list
		list_addTail( &(thread_ctrl->emptyList), &(all->threads[i].connect) );
	}

	// init idle thread
	all->idleThread.status = SLEEPING;
	for(i = 0;i<NR_OF_REGS;i++)
		all->idleThread.regs.r[i] = 0;
	all->idleThread.inital_sp = SP_EXTERNAL_RAM - MAX_THREADS*THREADSTACKSIZE;
	all->idleThread.regs.sp = all->idleThread.inital_sp;
	all->idleThread.regs.pc = (unsigned int)thread_idleThread;
	all->idleThread.regs.lr = (unsigned int)thread_idleThread;
	// threads run in user mode
	all->idleThread.regs.cpsr = 0x00000010;
	all->idleThread.pos = MAX_THREADS;
	thread_createID( &(all->idleThread) );
	all->idleThread.timestamp = 0;
	// init our queue values
	thread_ctrl->running = 0;
	
	return 1;
}

/* if THREAD_ENABLED in thread.h, 
 * sys-timer will call this sheduler, 
 * every time when dealing with an PIT-Interrupt occurs
 *
 * this sheduler:
 * - findes the next thread with active status in our thread array
 * - switches context of the currently running thread with the next active if such a thread
 * - turns on idle mode if there are no more active threads
 * nextActive means here : looking in all positions after the currently active thread
 * not a perfect FIFO, because a new thread will get a random position, and be the next running
 * but no thread should starve to death.. */
int thread_runSheduler( struct registerStruct * regStruct )
{

	if( list_isEmpty( &(thread_ctrl->activeList)) ){

		if( thread_ctrl->running ){
			if( thread_ctrl->running->status == DEAD || thread_ctrl->running->status == SLEEPING ){
				all->idleThread.status = ACTIVE;
				return  thread_switch( &(all->idleThread), regStruct);
			}
			if( thread_ctrl->running->status == RUNNING )
				return 1;
		}else{
			all->idleThread.status = ACTIVE;
			return  thread_switch( &(all->idleThread), regStruct);
		}

	}
	struct thread * nextActive = (struct thread *)list_popHead( &thread_ctrl->activeList);

	return thread_switch(nextActive , regStruct);
}

int thread_create( void * function, void * params, struct registerStruct * regStruct )
{
	if( list_isEmpty( &thread_ctrl->emptyList ) ){
		print("no space for another thread - return 0\n");
		return 0;
	}
	
	st_enablePIT();
	//print("\nbefore popHead : thread_ctrl->emptyList \n");
	//listIterate(&thread_ctrl->emptyList);
	struct thread * newThread = (struct thread *)list_popHead( &thread_ctrl->emptyList );
	//print("\nafter popHead : thread_ctrl->emptyList \n");
	//listIterate(&thread_ctrl->emptyList);
	//print("\nnewThread->pos %x\n", newThread->pos);

	newThread->status = ACTIVE;

	int i = 0;

	newThread->regs.r[0] = (unsigned int)params;
	for(i = 1;i<NR_OF_REGS;i++){
		newThread->regs.r[i] = 0;
	}

	// newThread
	// ->regs.pc <=> interrupt lr, where the new thread will continue
	// 
	newThread->regs.pc = (unsigned int)function;
	newThread->regs.lr = (unsigned int)thread_exit;
	newThread->regs.cpsr = 0x00000010;
	newThread->regs.sp = newThread->inital_sp;

	thread_createID(newThread);
	newThread->timestamp =  st_getTimeStamp();

	if( asm_isMode_IRQ() || asm_isMode_SVC() ){

		int out = thread_switch(newThread, regStruct );
		if(out == 1){
			return newThread->id;
		}
		print("##################################################\nERROR DURING THREAD_CREATE - DUNRING THREAD_SWITCH\n");
		return 0;
	}
	else{
		print("thread_create from sys or user");
		asm_swi_call_create(newThread);
	}
	return 0;
}

/* when an swi interrupt is triggered, determine here what to do with the running thread */
int thread_dealWithSWI(unsigned int swiCode, struct registerStruct * regStruct )
{
	switch(swiCode){
	case SWI_KILL :{
		return thread_kill(regStruct);
	}
	case SWI_CREATE :{
		return thread_newThread(regStruct);
	}
	case SWI_YIELD :{
		return thread_calledYield(regStruct);
	}
	default:
		print("unknow swi code %x\nthread.c could not deal with this swi \n",swiCode);
		printRegisterStruck(regStruct);
		return 0;
	}
}

/* enables a thread to end himself 
 * will perform an svc call */
int thread_exit( void )
{
	thread_ctrl->running->status = DEAD;
	asm_swi_call_kill();
	return 1;
}

/* enables a thread to active give up his timeslice 
 * will perform an svc call */
int thread_yield( void )
{
	asm_swi_call_yield();
	return 1;
}