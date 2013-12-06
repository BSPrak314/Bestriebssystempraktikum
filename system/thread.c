
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

int thread_sp[TOTAL_THREADSIZE]

static volatile
struct thread_queue * const thread_ctrl = (struct thread_queue *)THEAD_CTRL_BASE;

static volatile
struct threadArray * const allThreads = (struct threadArray *)THEAD_ARRAY_BASE;

int thread_initQueue( void )
{
	/*
	thread_ctrl.empty
	thread_ctrl.active
	thread_ctrl.sleeping = initList();
	*/
	thread_ctrl->curr_running = 0;
	thread_ctrl->curr_pos = -1;

	for(int i = 0;i<TOTAL_THREADSIZE-1;i++){	
			allThreads[i]->status = DEAD;
			thread_sp[i] = SP_INTERNAL_RAM - i*STACKSIZE;
	}
	return 1;
}

int thread_newThread( void * function_pointer )
{
	int newPos = 0;
	if( thread_ctrl->curr_pos > 0){
			newPos = thread_ctrl->curr_pos +1;
	}

	for(int i = 0;i<TOTAL_THREADSIZE;i++){
			if( newPos >= TOTAL_THREADSIZE-1 )
					newPos = 0;
			if( allThreads[newPos] ){
					if( allThreads[newPos]->status == DEAD ){
							break;
					}
			}
			newPos++;
	}

	if( allThreads[newPos] ){
			if( allThreads[newPos]->status != DEAD ){
					print("thread queue full");
					return -1;
			}
	}

	struct thread *newThread = allThreads[newPos];

	for(int i = 0;i<NR_OF_REGS;i++){
			newThread->reg[i] = 0;
	}

	newThread->sp = thread_sp[newPos];
	newThread->lr = (unsigned int)(&function_pointer);
	/* TP DO :
	 * get CPSR */
	unsigned int cpsr;
	newThread->cpsr

	newThread->status = ACTIVE;
	newThread->timestamp = st_getTimeStamp();
}

int thread_destroy( struct thread *deadThread )
{

	struct thread *tmp = 0;
	switch( deadThread->status ){
	case ACTIVE :{
			print("trying to destroy active thread");
			if( deadThread->next ){
					tmp = deadThread->next;
					tmp->prev = deadThread->prev;
			}
			if( deadThread->prev ){
					deadThread->prev->next = tmp;
			}
			return -1;
		}
	case SLEEPING :{
			print("trying to destroy sleeping thread");
			if( deadThread->next ){
					tmp = deadThread->next;
					tmp->prev = deadThread->prev;
			}
			if( deadThread->prev ){
					deadThread->prev->next = tmp;
			}
			return -1;
		}
	case RUNNING :{
			if( thread_ctrl->next_active ){
					thread_ctrl->running = thread_ctrl->next_active;
					thread_ctrl->next_active = thread_ctrl->next_active->next;
					thread_ctrl->next_active->prev = 0;
					thread_ctrl->running->next = 0;
					thread_ctrl->running->prev = 0;
			}else{
				/* switch to idle mode*/
				;
			}
			return 1;
		}
	case DEAD :
	default :
			print("trying to destroy dead thread");
			return -1;
	}
}

void asm_sysRegisterToStack( unsigned int [] )
{
	saveRegisterInArray
}

void saveRegisterInArray( unsigned int )
{

}

int thread_sleep( unsigned int thread_pos )
{

}

int thread_wakeUp( unsigned int thread_pos )
{

}

int startIdle( void )
{
	st_setPeriodicValue(0x0);
	return -1;
}

int endIdle( void )
{
	st_setPeriodicValue(timeSlice);
	return -1;
}