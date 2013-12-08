
#include <thread.h>
#include <dbgu.h>
#include <utils.h>
#include <printf.h>
#include <sys_timer.h>
#include <pmc.h>
#include <reg_operations_asm.h>
#include <exception_handler.h>


#define TIMESLICE 0x00000800

/* array holding the base addresses for our thread */
unsigned int thread_sp[TOTAL_THREADSIZE];
/* array holding registers during contextchange */
unsigned int tmp_regs[NR_OF_REGS+3];
/* flag to enable thread test with dummy threads, shared with dbgu */
unsigned int thread_test = 0;
/* message to print during context change, printed via buffered output */
char* infoContextChange = 0; 

/* pointer to thread controll, hardcoded address, directly after IVT */
static 
struct thread_queue * const thread_ctrl = (struct thread_queue *)THEAD_CTRL_BASE;
/* pointer to thread array, hardcoded address, directy after thread controll */
static 
struct threadArray * const all = (struct threadArray *)THEAD_ARRAY_BASE;

/* we use reg_operatons_asm to get the registers from the running thread
 * namly the function asm_saveSysRegisterToTmp from reg_operations 
 * asm_saveSysRegisterToTmp writes relevant registers to stack 
 * and the calls 
 -> saveRegisterInArray
 * gets the address where registers from current thread where saved on the stack
 * and stores them in our tmp array  */
void saveRegisterInArray( unsigned int entry )
{
	int i = 0;
	for(i=NR_OF_REGS+2;i>=0;i--){
		tmp_regs[i] = (unsigned int)&entry;
		entry+=4;
	}
}

/* return -1 if our thread queue holds no more active threads 
 * return array position for next active thread in queue otherwise */
static int thread_nextActive( void )
{
	int nextActive = 0;
	// starts searching after the position of our running thread
	//  if there is an running thread
	if( thread_ctrl->curr_running->status != DEAD){
		nextActive = thread_ctrl->curr_pos +1;
		// avoid modulo operatins
		if(nextActive >= TOTAL_THREADSIZE)
			nextActive = 0;
	}
	int i = 0;
	for(i = 0;i<TOTAL_THREADSIZE;i++){
		// found active thread
		if( all->threads[nextActive].status == ACTIVE ){
			if( thread_ctrl->nr_activeThreads == 0)
				print("Error in nr_activeThreads - should be positive!\n");
			return nextActive;
		}
		nextActive++;
		// avoid modulo operations
		if( nextActive >= TOTAL_THREADSIZE )
			nextActive = 0;
	}
	// found no active thread
	if( thread_ctrl->nr_activeThreads != 0)
		print("Error in nr_activeThreads - should be zero!\n");
	return -1;
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
	/*
	thread_ctrl.empty
	thread_ctrl.active
	thread_ctrl.sleeping = initList();
	*/
	st_setPeriodicValue(TIMESLICE);
	thread_ctrl->curr_running = &(all->threads[0]);
	thread_ctrl->curr_pos = 0;
	thread_ctrl->nr_activeThreads = 0;
	thread_ctrl->nr_sleepingThreads = 0;
	int i = 0;
	for(i = 0;i<TOTAL_THREADSIZE-1;i++){	
		all->threads[i].status = DEAD;
		thread_sp[i] = SP_INTERNAL_RAM - i*STACKSIZE;
	}
	return 1;
}

/* if THREAD_ENABLED in thread.h, 
 * sys-timer will call this sheduler, 
 * every time when dealing with an PIT-Interrupt
 *
 * this sheduler:
 * - findes the next thread with active status in our thread array
 * - switches context of the currently running thread with the next active if such a thread
 * - turns on idle mode if there are no more active threads
 * nextActive means here : looking in all positions after the currently active thread
 * not a perfect FIFO, because a new thread will get a random position, and be the next running
 * but no thread should starve to death.. */
int thread_runSheduler( void )
{
	int newActive = thread_nextActive();
	// no more active threads
	if( newActive < 0 ){
		// if our running thread is still running - nothing to change
		if( thread_ctrl->curr_running->status == RUNNING ){
			return 1;
		// else turn on idle mode
		}else{
			print("all threads done - starting idle thread");
			thread_enableThreads();
			idle_thread();
			return 1;
		}
	}
	// found active thread to switch to
	thread_switch( newActive );

	return 1;
}

/* switching running thread with new active thread
 * if running thread is not DEAD
 * - saving registers from running thread to tmp register
 * - store register from running thread in coresponding thread struct 
 * - set status from running to active
 * then 
 * - puts all register from new running thread to tmp register
 * - replace register values on stack with values from new running thread
 * so when this interrupt is done, new running thread will continue */
int thread_switch( int newActive )
{
	// notify about context change
	printf(infoContextChange);
	
	if(newActive < 0){
		print("Error in thread sheduling - trying to switch to non exsisting thread!! \n");
		return -1;
	}

	int i = 0;
	 // if running thread is not DEAD
	if(thread_ctrl->curr_running->status != DEAD ){
		// get register values from old running thread and saves them in tmp
		asm_saveSysRegisterToTmp();
		
		// store register values from old running in thread struct
		for(i = 0;i<NR_OF_REGS;i++)
			thread_ctrl->curr_running->reg[i] = tmp_regs[i];
		thread_ctrl->curr_running->lr = tmp_regs[NR_OF_REGS];
		thread_ctrl->curr_running->sp = tmp_regs[NR_OF_REGS+1];
		thread_ctrl->curr_running->cpsr = tmp_regs[NR_OF_REGS+2];

		// old running is now an active thread - if oöd running ís sleeping, let him sleep
		if( thread_ctrl->curr_running->status == RUNNING )
			thread_ctrl->curr_running->status = ACTIVE;
		// updating timestamp to CurrentRealTimeRegister Value
		thread_ctrl->curr_running->timestamp = st_getTimeStamp();
	}
	// set new running thread, by updating curr_pos and curr_running in thread_ctrl
	thread_ctrl->curr_pos = newActive;
	thread_ctrl->curr_running = &(all->threads[newActive]);
	// updating status flag for new running thread
	thread_ctrl->curr_running->status = RUNNING;
	// load register values from new running thread in tmp array
	for(i = 0;i<NR_OF_REGS;i++)
		tmp_regs[i] = thread_ctrl->curr_running->reg[i];
	tmp_regs[NR_OF_REGS] = thread_ctrl->curr_running->lr;
	tmp_regs[NR_OF_REGS+1] = thread_ctrl->curr_running->sp;
	tmp_regs[NR_OF_REGS+2] = thread_ctrl->curr_running->cpsr;
	// updating stored register values on sp_irq 
	// so when leaving interrupt handler, we switch to the new running thread
	asm_loadSysRegisterFromTmp(tmp_regs);
	
	return 1;
}

/* inits a new thread and switching this new thread to next running thread when this interrupt is done
 * giving the new thread the rest of the timeslice of the old running thread
 * about new thread:
 * - registers r0-r12 are set to 0
 * each possible thread in our thread queue has an own stackpointer, so
 * - sp is set to hardcoded baseaddress for the position of the new thread in our queue
 * - lr is set the function the thread will start at 
 * - cpsr is set the to current cpsr,
 * except for the following modifications:
 * N = 0, Z = 0, C = 0, V = 0, I = 0, M[4:0] = 11111 (System)
 *
 * return -1 and print a message, if threadQueue is full
 * otherwise 
 * return result for switching the new Thread to running */
int thread_start( void * function_pointer )
{
	// determine the position in our thread queue 
	// for an easy FIFO, starts looking after our running thread
	int newPos = 0;
	if( thread_ctrl->curr_pos > 0){
			newPos = thread_ctrl->curr_pos +1;
	}
	int i = 0;
	for(i = 0;i<TOTAL_THREADSIZE;i++){
		// found position for new thread
		if( all->threads[newPos].status == DEAD ){
			break;
		}
		newPos++;
		// avoid modulo operation
		if( newPos >= TOTAL_THREADSIZE )
			newPos = 0;
	}
	// have we found no empty spot in the thread queue
	if( all->threads[newPos].status != DEAD ){
		print("thread queue full\n");
		return 0;
	}
	// one more active thread now
	thread_ctrl->nr_activeThreads++;

	// now point to the empty spot and adjust all variables
	struct thread *newThread = &(all->threads[newPos]);

	for(i = 0;i<NR_OF_REGS;i++){
		newThread->reg[i] = 0;
	}
	// fresh stackpointer
	newThread->sp = thread_sp[newPos];
	// when this interrupt is done,
	// this will be the address the new thread will jump to
	newThread->lr = (unsigned int)(&function_pointer);
	// an vanilla cpsr for our thread, will be stored in irq_spsr
	// when the thread will switched to running
	newThread->cpsr = asm_getSPSRforNewThread();
	// even when this thread will be running soon , he is just active for now
	newThread->status = ACTIVE;
	// CurrentRealTimeRegister Value - and we are done
	newThread->timestamp = st_getTimeStamp();
	// switch the new thread to running
	return thread_switch(newPos);
}

/* this funcition is called from system/usr mode - so we have to throw an exception to initiate a thread switch 
 * calling a swi with the kill instruction will do the trick 
 * CALL_KILL_SWI is defined in exception_handler.h */
void thread_close( void )
{
	asm(CALL_KILL_SWI:::);
}

/* sets the status of the currently running thread to DEAD
 * then switches the DEAD running thread with next active thread 
 *
 * return -1 if trying to kill a non running thread
 * return 1 if last running thread was killed and idle mode is entered
 * return result of switching to next active thread otherwise */
int thread_kill( void )
{
	if(thread_ctrl->curr_running == 0){
		print("trying to kill thread with no non-running thread\n");
		print("means sheduler is running wild !! \n");
		return -1;
	}
	if( thread_ctrl->curr_running->status != RUNNING){
		print("trying to kill non-running thread\n");
		print("means curr_running is not a running thread !! \n");
		return -1;
	}
	// the actuall killing
	thread_ctrl->curr_running->status = DEAD;
	thread_ctrl->nr_activeThreads--;

	// current thread is dead, next thread deserves his full timeslice
	st_setPeriodicValue(TIMESLICE);


	// no more active threads means idle mode - otherwise thread_switch
	// entering via thread_sheduler would save code lines
	// entering by writing 3 lines double saves 2 jumps...
	//return thread_runSheduler();
	// avoid some jumps...

	int next = thread_nextActive();
	if( next < 0 ){
		if( thread_ctrl->nr_activeThreads != 0)
			print("Error in nr_activeThreads - should be zero after killing!\n");
		print("all threads done - starting idle thread");
		thread_enableThreads();
		idle_thread();
		return 1;
	}
	// switch the next active thread
	if( thread_ctrl->nr_activeThreads == 0)
		print("Error in nr_activeThreads - at least one left!\n");
	return thread_switch( next );
}

/* disables PIT Interrupts, 
 * disablesProcessorClock <=> StandbyMode enabled
 * and set the flag in thread_ctrl */
int thread_startIdle( void )
{
	pmc_disableProcessorClock();
	st_disablePIT();
	return 1;
}

/* enables PIT Interrupts, reload TIMESLICE as PIT Value
 * and set the flag in thread_ctrl */
int thread_endIdle( void )
{
	st_setPeriodicValue(TIMESLICE);
	st_enablePIT();
	return 1;
}

/* int thread_test is shared with dbgu
 * if thread_test != 0 this will be called 
 * every time dbgu deals with an TXRDY Interrupt */
void thread_testContextChange( void )
{
	thread_start( dummy_thread );
}

/* Our dummy thread
 * will print out the next character from the input buffer 14-time
 * then kill itself */
void dummy_thread( void )
{
	char c = 0;
	// get the input to print
	while( c == 0){
		if( dbgu_hasBufferedOutput() ){
			c = dbgu_nextInputChar();
		}
	}
	int i = 0;
	for(i = 0;i<14;i++){
		printf("%c",c);
		waitBusy(800000);
	}
	// thread done - let init thread_kill via swi
	thread_close();
}

/* Our idle thread
 * disables PIT Interrupts and starts endless loop,
 * will be disabled if an RXRDY Interrupt starts a new thread */
void idle_thread( void )
{
	thread_startIdle();
	while( 1 )
		;
}

/* threads run in usr/system mode and any context change is performed during interrupts
 * so an thread will call an SWI to kill himself, put himself to sleep or wait for defined time without active waiting 
 * the SWI call will contain the instruction, the thread is trying to perform - if its waiting, we need more information
 * SWI_KILL, SWI_SLEEP, SWI_WAIT and other swi flags defined in exception_handler.h */
void thread_dealWithSWI( unsigned int instr, unsigned int r0 )
{
	if(instr == SWI_KILL){
		thread_kill();
	}
	if(instr == SWI_SLEEP){
		// current thread is going to sleep, next thread deserves his full timeslice
		st_setPeriodicValue(TIMESLICE);
		// put thread to sleep
		thread_ctrl->curr_running->status = SLEEPING;
		thread_ctrl->curr_running->sleeptime = 0xFFFFFF;
		thread_ctrl->curr_running->wakeUpCode = SWI_SLEEP;
		// inform the control struct about thread numbers
		thread_ctrl->nr_activeThreads--;
		thread_ctrl->nr_sleepingThreads++;
		// let the sheduler find out how to handle the situation
		thread_runSheduler();
	}
	if(instr == SWI_WAIT_TIME){
		// current thread is going to sleep, next thread deserves his full timeslice
		st_setPeriodicValue(TIMESLICE);
		// put thread to sleep 
		thread_ctrl->curr_running->status = SLEEPING;
		thread_ctrl->curr_running->wakeUpCode = SWI_WAIT_TIME;
		// setting the wakeUp clock...
		unsigned int wakeUpTime = st_getTimeStamp();
		wakeUpTime = wakeUpTime + r0;
		thread_ctrl->curr_running->sleeptime = wakeUpTime;
		st_enableAlarmInterrupt();
		// inform the control struct about thread numbers
		thread_ctrl->nr_activeThreads--;
		thread_ctrl->nr_sleepingThreads++;
		// let the sheduler find out how to handle the situation
		thread_runSheduler();
	}
}

void thread_wakeUp( void )
{
	st_disableAlarmInterrupt();
}