
#include <thread.h>
#include <printf.h>
#include <sys_timer.h>
#include <exception_handler_asm.h>

unsigned int thread_sp[TOTAL_THREADSIZE];
unsigned int tmp_regs[NR_OF_REGS+3];

static volatile
struct thread_queue * const thread_ctrl = (struct thread_queue *)THEAD_CTRL_BASE;

static volatile
struct threadArray * const all = (struct threadArray *)THEAD_ARRAY_BASE;

int thread_initQueue( void )
{
	/*
	thread_ctrl.empty
	thread_ctrl.active
	thread_ctrl.sleeping = initList();
	*/
	thread_ctrl->curr_running = 0;
	thread_ctrl->curr_pos = -1;
	int i = 0;
	for(i = 0;i<TOTAL_THREADSIZE-1;i++){	
			all->threads[i].status = DEAD;
			thread_sp[i] = SP_INTERNAL_RAM - i*STACKSIZE;
	}
	return 1;
}

int thread_baseSheduler( void )
{
	int newActive = 0;
	if( thread_ctrl->curr_pos > 0){
			newActive = thread_ctrl->curr_pos +1;
	}
	int i = 0;
	for(i = 0;i<TOTAL_THREADSIZE;i++){
			if( newActive >= TOTAL_THREADSIZE-1 )
					newActive = 0;
			if( all->threads[newActive].status == ACTIVE ){
					break;
			}
			newActive++;
	}

	if( all->threads[newActive].status != ACTIVE ){
			return 1;
	}
	thread_switch( newActive );

	return 1;
}

int thread_switch( unsigned int newActive )
{
	printf("\n");
	asm_saveSysRegisterToTmp();
	int i = 0;
	for(i = 0;i<NR_OF_REGS;i++)
		thread_ctrl->curr_running->reg[i] = tmp_regs[i];
	
	thread_ctrl->curr_running->lr = tmp_regs[NR_OF_REGS];
	thread_ctrl->curr_running->sp = tmp_regs[NR_OF_REGS+1];
	thread_ctrl->curr_running->cpsr = tmp_regs[NR_OF_REGS+2];
	thread_ctrl->curr_running->status = ACTIVE;
	thread_ctrl->curr_running->timestamp = st_getTimeStamp();

	thread_ctrl->curr_pos = newActive;
	thread_ctrl->curr_running = &(all->threads[newActive]);
	
	for(i = 0;i<NR_OF_REGS;i++)
		tmp_regs[i] = thread_ctrl->curr_running->reg[i];
	tmp_regs[NR_OF_REGS] = thread_ctrl->curr_running->lr;
	tmp_regs[NR_OF_REGS+1] = thread_ctrl->curr_running->sp;
	tmp_regs[NR_OF_REGS+2] = thread_ctrl->curr_running->cpsr;
	
	asm_loadSysRegisterFromTmp(tmp_regs);

	return 1;
}

struct thread * thread_newThread( void * function_pointer )
{
	int newPos = 0;
	if( thread_ctrl->curr_pos > 0){
			newPos = thread_ctrl->curr_pos +1;
	}
	int i = 0;
	for(i = 0;i<TOTAL_THREADSIZE;i++){
			if( newPos >= TOTAL_THREADSIZE-1 )
					newPos = 0;
			
			if( all->threads[newPos].status == DEAD ){
					break;
			}
			newPos++;
	}

	if( all->threads[newPos].status != DEAD ){
			print("thread queue full");
			return 0;
	}

	struct thread *newThread = &(all->threads[newPos]);

	for(i = 0;i<NR_OF_REGS;i++){
			newThread->reg[i] = 0;
	}

	newThread->sp = thread_sp[newPos];
	newThread->lr = (unsigned int)(&function_pointer);
	
	/* T0 DO :
	 * get CPSR */
	unsigned int cpsr = 0;
	newThread->cpsr = cpsr;
	// ERROR if not completet !!!!

	newThread->status = ACTIVE;
	newThread->timestamp = st_getTimeStamp();
	return newThread;
}

int thread_destroy( struct thread *deadThread )
{
	switch( deadThread->status ){
	case ACTIVE :{
			print("trying to destroy active thread");
			deadThread->status = DEAD;
			return -1;
		}
	case SLEEPING :{
			print("trying to destroy sleeping thread");
			deadThread->status = DEAD;
			return -1;
		}
	case RUNNING :{
			deadThread->status = DEAD;
			return 1;
		}
	case DEAD :
	default :
			print("trying to destroy dead thread");
			return -1;
	}
	return -1;
}

void saveRegisterInArray( unsigned int entry )
{
	int i = 0;
	for(i=NR_OF_REGS+2;i>=0;i--){
		tmp_regs[i] = (unsigned int)&entry;
		entry+=4;
	}
}
/*
int thread_sleep( unsigned int thread_pos )
{
	return -1;
}

int thread_wakeUp( unsigned int thread_pos )
{
	return -1;
}
*/
int startIdle( void )
{
	st_setPeriodicValue(0x0);
	return -1;
}

int endIdle( void )
{
	st_setPeriodicValue(TIMESLICE);
	return -1;
}