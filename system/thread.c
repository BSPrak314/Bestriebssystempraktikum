
#include <list.h>
#include <thread.h>
#include <dbgu.h>
#include <utils.h>
#include <printf.h>
#include <sys_timer.h>
#include <pmc.h>
#include <interrupt_handler.h>
#include <reg_operations_asm.h>
#include <mem_ctrl.h>
#include <mmu_asm.h>

#define TIMESLICE 		400
#define with_VIRTUAL_STACK      1

/* thread control structure */
static
struct threadQueue * const thread_ctrl = (struct threadQueue *)THEAD_CTRL_BASE;
/* thread array */
static
struct add_Space_ctrl * const add_space_ctrl = (struct add_Space_ctrl *)SPACE_CTRL_BASE;

static
struct threadArray * const all = (struct threadArray *)THEAD_ARRAY_BASE;


static void thread_idleThread( void )
{
	/*
	while(1){
		pmc_disableProcessorClock();
		st_disablePIT();
	*/
		idle();
	//}
}

/* each threads get an individual id 
 * - first  8 bit for the position in our thread queue 
 * - last  24 bit for the timestamp when the thread was created */
static void thread_createID( struct thread * thread )
{
	thread->id = st_getTimeStamp();
	thread->id &= 0x00ffffff;
	thread->id += (thread->pos << 24);
}

/* init Idle Thread struct */
static void thread_initIdleThread( void )
{
	int i = 0;
	// idle is default running thread -> creating the first threads sets thread_sheduler_enabled to 1
	// and idle thread is in the game
	all->idleThread.status = SLEEPING;
	for(i = 0;i<NR_OF_REGS;i++)
		all->idleThread.regs.r[i] = 0;
	
	all->idleThread.inital_sp = USERSTACK_BASE + (MAX_THREADS +1)*THREAD_STACKSIZE;
			
	all->idleThread.regs.sp = all->idleThread.inital_sp;
	all->idleThread.regs.pc = (unsigned int)&thread_idleThread;
	all->idleThread.regs.lr = (unsigned int)&thread_idleThread;
	all->idleThread.regs.cpsr = 0x0000001f;
	all->idleThread.pos = MAX_THREADS;
	all->idleThread.address_Space = 15;
	thread_createID( &(all->idleThread) );
	all->idleThread.timestamp = st_getTimeStamp();
}

/* only thread_runSheduler, thread_create and thread_kill 
 * call for
 * thread_contextChange when - there are 2 or more active threads
 *                    - at the moment when threads get enabled, to switch to idle thread 
 *                    - the first active thread arrives to wake from idle
 *                    - the last active thread is dead, to switch to idle */
static int thread_contextChange( struct thread * newThread, struct registerStruct * regStruct )
{
	// running thread is not dead - save its register values, in coresponding thread struct
	if(thread_ctrl->running->status != DEAD && thread_ctrl->running->pos != MAX_THREADS){
		memcpy( &(thread_ctrl->running->regs), regStruct, SIZE_OF_REGISTER_STRUCT);
	}
	// now we put the former running thread in the right list
	if(thread_ctrl->running->status == SLEEPING || thread_ctrl->running->status == WAIT_INPUT ){
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
	memcpy(regStruct, &(newThread->regs), SIZE_OF_REGISTER_STRUCT);

	// update the information in our thread_ctrl struct
	thread_ctrl->running = newThread;
	thread_ctrl->running->status = RUNNING;
	thread_ctrl->running->timestamp = st_getTimeStamp();

	mc_fastContextSwitch(thread_ctrl->running->address_Space);

	return thread_ctrl->running->id;
}

/* only thread_exit calls this function via SWI-interrupt
 * this function performs a kill, be setting the status of the current thread to DEAD
 * next thread get his full timeslice, next thread will be determined by our sheduler */
int thread_kill( struct registerStruct * regStruct )
{	
	struct thread * deadThread = thread_ctrl->running;

	deadThread->status = DEAD;

	unsigned int domain = deadThread->address_Space;

	mc_disableStack_forThread(deadThread->pos, domain);

	add_space_ctrl->spaces[domain-1].running_threads--;

	if( !add_space_ctrl->spaces[domain-1].running_threads ){
		int i = 0;
		for(;i<NR_OF_EMPTYPAGES;i++){
			if( add_space_ctrl->emptyPages[i] == domain )
				add_space_ctrl->emptyPages[i] = 0;
		}
	}

	st_setPeriodicValue(TIMESLICE);
	return thread_runSheduler(regStruct, 0);
}

static int nextPage(unsigned int domain)
{
	int i = 0;
	for(;i<NR_OF_EMPTYPAGES;i++){
		if( !add_space_ctrl->emptyPages[i])
			break;
	}
	if( i == NR_OF_EMPTYPAGES )
		return -1;
	add_space_ctrl->emptyPages[i] = domain;
	return i;
}

static int thread_needsMemory( void )
{
	int page = nextPage(thread_ctrl->running->address_Space);
	if( page < 0){
		print("thread called: more Memory\n");
		print("out of Memory\n");
		return 0;
	}
	add_space_ctrl->spaces[thread_ctrl->running->address_Space-1].pages++;
	return mc_allocMemory(page,thread_ctrl->running->address_Space, add_space_ctrl->spaces[thread_ctrl->running->address_Space-1].pages );
}

/* thread_wait calls this funciton via SWI-interrupt
 * -> putting this thread->status to sleep
 * -> enables AlarmInterrupt
 * -> sets AlarmClockTime and saves this value as wakeUpTime */
 static int thread_calledWait( struct registerStruct * regStruct, unsigned int msec )
{
	thread_ctrl->running->wakeTime = st_setAlarm(msec);
	thread_ctrl->running->status = SLEEPING;
	st_setPeriodicValue(TIMESLICE);
	st_enableAlarmInterrupt();
	return thread_runSheduler(regStruct, 0);
}

/* thread_yield calls this funciton via SWI-interrupt
 * -> calls the sheduler, depending on 
 * when thread is still active -> at the end of active queue OR running if its the only active thread
 *             is sleeping     -> at the end of the sleeping queue
 *             is dead         -> to his grave BUT will not kill the thread !!  */
static int thread_calledYield( struct registerStruct * regStruct )
{
	st_setPeriodicValue(TIMESLICE);
	return thread_runSheduler(regStruct, 0);
}

/* current running thread called asm_swi_read
 * put current running thread to sleep, until char is recieved */
static int thread_calledRead( struct registerStruct * regStruct )
{
	if( dbgu_hasBufferedInput() ){
		char c = dbgu_nextInputChar();
		regStruct->r[0] = (int)c;
		return (int)c;
	}
	// syscall_readChar will wait 10 minutes for an char
	thread_ctrl->running->wakeTime = st_setAlarm(600000);
	thread_ctrl->running->status = WAIT_INPUT;
	st_setPeriodicValue(TIMESLICE);
	st_enableAlarmInterrupt();
	thread_runSheduler(regStruct, 0);
	return 0;
}

/* current running thread called asm_swi_write
 * -> print character - buffered Output */
static int thread_calledWrite( struct registerStruct * regStruct )
{
	print("%c", (char)(regStruct->r[0]) );
	return 1;
}

static int thread_newThread( struct registerStruct * regStruct, unsigned int function, unsigned int params, unsigned int domain )
{
	if( list_isEmpty( &thread_ctrl->emptyList ) ){
		print("no space for another thread - return 0\n");
		return 0;
	}
	st_enablePIT();

	struct thread * newThread = (struct thread *)list_popHead( &thread_ctrl->emptyList );
	newThread->status = ACTIVE;

	int i = 0;
	newThread->regs.r[0] = params;
	for(i = 1;i<NR_OF_REGS;i++){
		newThread->regs.r[i] = 0;
	}
	
	newThread->regs.pc = function;
	newThread->regs.lr = 0;
	newThread->regs.cpsr = 0x00000010;

	newThread->address_Space = domain;
	add_space_ctrl->spaces[domain-1].running_threads++;
	
	if( with_VIRTUAL_STACK ){
		newThread->regs.sp = 0x100000 - add_space_ctrl->spaces[domain-1].running_threads * THREAD_STACKSIZE;
	}
	else
		newThread->regs.sp = newThread->inital_sp;
	
	newThread->local_id = add_space_ctrl->spaces[domain-1].running_threads;

	thread_createID(newThread);
	mc_enableStack_forThread(newThread->pos, domain);
	newThread->timestamp =  st_getTimeStamp();
	
	return thread_contextChange(newThread, regStruct);
}

static int thread_newProcess( struct registerStruct * regStruct, unsigned int function, unsigned int params )
{
	int i = 0;
	for(i = 0;i<MAX_ADD_SPACES;i++){
		if( ! add_space_ctrl->spaces[i].running_threads ){
			break;
		}
	}
	unsigned int domain = (i+1);
	if( i >= MAX_ADD_SPACES ){
		print("maximum number of processes running  - return 0\n");
		return 0;
	}
	if( list_isEmpty( &thread_ctrl->emptyList ) ){
		print("no space for another thread - return 0\n");
		return 0;
	}

	int page = nextPage(domain);
	if( page < 0){
		print("out of memory  - return 0\n");
	}
	mc_allocMemory(page,domain,1);
	return thread_newThread( regStruct, function, params, domain );
}

/* if thread_sheduler_enabled != 0 in thread.c, 
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
int thread_runSheduler( struct registerStruct * regStruct, unsigned int alarm )
{
	if( list_isEmpty( &(thread_ctrl->activeList)) ){
		if( !thread_wakeUp() ){
			// st_disablePIT();
			if( thread_ctrl->running->status != RUNNING ){
				all->idleThread.status = ACTIVE;
				return  thread_contextChange( &(all->idleThread), regStruct);
			}
		}
		if( thread_ctrl->running->status == RUNNING ){
			return 1;
		}
		print("ERROR !!!!!!\n!!!!!!!!!!!\n");
	}
	// switch to next active thread pending
	if( alarm && all->idleThread.status != RUNNING )
		return 1;
	struct thread * nextActive = (struct thread *)list_popHead( &thread_ctrl->activeList);
	return thread_contextChange(nextActive , regStruct);
}

/* init our thread control structure, by
 * - sets the base-address for stackpointer for all thread in queue
 * - sets status of all thread in our queue to DEAD <=> empty queue
 * - sets pointer to running thread to start of thread queue and curr_pos = 0
 * so when switching to our first thread, curr_running is DEAD and
 * thread_contextChange knows there are no register to save from previous thread 
 * - sets the PeriodicTimerIntervall to our TIMESLICE, so each PIT inducates a contextchange */
int thread_initThreadControl( void )
{
	st_setPeriodicValue(TIMESLICE);

	// clear Memory 
	// begin at threadQueue struct and over the whole threadArray struct
	unsigned int endValue = THEAD_CTRL_BASE + SIZE_OF_THREADCTRL + SIZE_OF_SPACECTRL + (MAX_THREADS+1) * SIZE_OF_THREAD;
	clearMemory( THEAD_CTRL_BASE, endValue);

	list_initList(&(thread_ctrl->sleepingList) );
	list_initList(&(thread_ctrl->emptyList) );
	list_initList(&(thread_ctrl->activeList) );

	int i = 0;
	
	for(i = 0;i<MAX_THREADS;i++){
		// all threads starts dead <=> this array position is empty
		all->threads[i].status = DEAD;
		// inital stack pointer for this array position - will never change
		if( ! with_VIRTUAL_STACK )
			all->threads[i].inital_sp = USERSTACK_BASE + (i+1)*THREAD_STACKSIZE;
		//
		// the current stackspointer is the inital stack pointer for this thread position
		all->threads[i].regs.sp = all->threads[i].inital_sp;
		// position of this thread in threadArray
		all->threads[i].pos = i;
		// all threads to the empty list
		list_addTail( &(thread_ctrl->emptyList), &(all->threads[i].connect) );
	}

	for(i = 0;i<MAX_ADD_SPACES;i++){
		add_space_ctrl->spaces[i].running_threads  = 0;
		add_space_ctrl->spaces[i].domain  = (i+1);
	}
	for(i = 0;i<NR_OF_EMPTYPAGES;i++){
		add_space_ctrl->emptyPages[i] = 0;
	}

	thread_initIdleThread();
	// init our queue values
	thread_ctrl->running = &all->idleThread;
	thread_ctrl->running->status = RUNNING;
	thread_ctrl->curr_IO = 0;
	return 1;
}

/* when an swi interrupt is triggered, determine here what to do with the running thread */
int thread_dealWithSWI(unsigned int swiCode, struct registerStruct * regStruct )
{
	switch(swiCode){
	case SWI_KILL :{
		return thread_kill(regStruct);
	}
	case SWI_FORK :{
		if( (regStruct->cpsr & 0x0000001f) != 0x10){
			print("threads can only be created in USER mode\nplease start a new process\nEXIT...\n");
			return 0;
		}
		return thread_newThread(regStruct, regStruct->r[0], regStruct->r[1], thread_ctrl->running->address_Space );
	}
	case SWI_NEWPROCESS :{
		return thread_newProcess(regStruct, regStruct->r[0], regStruct->r[1] );
	}
	case SWI_YIELD :{
		return thread_calledYield(regStruct);
	}
	case SWI_WAIT :{
		return thread_calledWait(regStruct, regStruct->r[0]);
	}
	case SWI_READ :{
		return thread_calledRead(regStruct);
	}
	case SWI_WRITE :{
		return thread_calledWrite(regStruct);
	}
	case SWI_GET_ID :{
		regStruct->r[0] = thread_ctrl->running->id;
		return thread_ctrl->running->id;
	}
	case SWI_GET_LOCALID :{
		regStruct->r[0] = thread_ctrl->running->local_id;
		return thread_ctrl->running->local_id;
	}
	case SWI_GET_MEMORY :{
		return thread_needsMemory();
	}
	default:
		print("unknow swi code %x\nthread.c could not deal with this swi \n",swiCode);
		print_RegisterStruct(regStruct);
		return 0;
	}
}

/* an wakeUp call 
 *  - will only performed after an AlarmInterrupt via st_dealWithAlarmInterrupt
 *  OR during sheduler, before switch to idle thread
 *  NORMALY i would add any sleepingThread with an wakeUpTime < currentTimeStamp  to the head of the active list 
 *  and send the sleepingThread, who has the lowest wakeUpTime running, 
 *  BUT we have long TIMESLICES to optical see what our threads are doing 
 *  - so this whould enable a wait(1) bomb and other thread could starve to death... 
 *  so this behaviour depends on CRITICAL_WAIT_TIME */
int thread_wakeUp(void)
{
	unsigned int currTime = st_getTimeStamp();
	unsigned int nextAlarm = 0;
	unsigned int out = 0;
	
	struct list * tmp = list_popHead( &thread_ctrl->sleepingList );
	
	struct list tmpList = list_newListStruct();
	list_clean(&tmpList);

	while( tmp ){
		struct thread * sleepingThread = (struct thread *)tmp;
		tmp = list_popHead( &thread_ctrl->sleepingList );
		
		if( sleepingThread->wakeTime <= currTime ){
			
			if( sleepingThread->status == WAIT_INPUT )
				sleepingThread->regs.r[0] = 0;
			
			sleepingThread->status = ACTIVE;

			list_addHead(&thread_ctrl->activeList, (struct list *)sleepingThread );
			out = 1;
			
		}else{
			list_addTail(&tmpList, (struct list *)sleepingThread );
			if( !nextAlarm )
				nextAlarm = sleepingThread->wakeTime;
			if( nextAlarm > sleepingThread->wakeTime)
				nextAlarm = sleepingThread->wakeTime;
		}
	}

	if( nextAlarm )
		st_setAlarmValue(nextAlarm);

	tmp = list_popHead( &tmpList );
	while(tmp){
		list_addTail( &thread_ctrl->sleepingList, tmp );
		tmp = list_popHead( &tmpList );
	}
	
	if( list_isEmpty(&thread_ctrl->sleepingList) ){
		st_disableAlarmInterrupt();
	}

	return out;
	//return thread_runSheduler(regStruct, 0);
}

unsigned int thread_getRunningPosition( void )
{
	return thread_ctrl->running->pos;
}

int thread_infoAboutInput( struct registerStruct * regStruct )
{
	if( list_isEmpty( &thread_ctrl->sleepingList ) ){
		return 1;
	}
	
        if( ! dbgu_hasBufferedInput() ){
        	return 0;
        }

        struct list tmpList = list_newListStruct();
        list_clean(&tmpList);

	struct list * tmp = list_popHead( &thread_ctrl->sleepingList );
	struct thread * nextRunning = 0;

	unsigned int nextAlarm = 0;
	unsigned int currTime = st_getTimeStamp();

	while( tmp ){
		struct thread * sleepingThread = (struct thread *)tmp;
		tmp = list_popHead( &thread_ctrl->sleepingList );

		if( sleepingThread->wakeTime > currTime ){
			if( !nextAlarm )
				nextAlarm = sleepingThread->wakeTime;
			if( nextAlarm > sleepingThread->wakeTime)
				nextAlarm = sleepingThread->wakeTime;
		}

		if( sleepingThread->status == WAIT_INPUT ){
			if( !nextRunning ){
				nextRunning = sleepingThread;
			}else{
				if( nextRunning->timestamp < sleepingThread->timestamp ){
					list_addHead(&thread_ctrl->sleepingList, (struct list *)sleepingThread );
				}else{
					list_addHead(&thread_ctrl->sleepingList, (struct list *)nextRunning );
					nextRunning = sleepingThread;
				}
			}
		}else{
			if( sleepingThread->wakeTime > currTime )
				list_addHead(&thread_ctrl->activeList, (struct list *)sleepingThread );
			else
				list_addTail(&tmpList, (struct list *)sleepingThread );
		}
	}
	tmp = list_popHead( &tmpList );
	while(tmp){
		list_addTail( &thread_ctrl->sleepingList, tmp );
		tmp = list_popHead( &tmpList );
	}

	if( nextAlarm )
		st_setAlarmValue(nextAlarm);
	
	if( nextRunning ){
		nextRunning->regs.r[0] = (int) dbgu_nextInputChar();
		return thread_contextChange(nextRunning, regStruct);
	}
	return 0;
}