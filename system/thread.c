
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

#define TIMESLICE 0x00000800

struct thread{
	unsigned int prev;
	unsigned int next;
	unsigned int reg[12];
	unsigned int lr;
	unsigned int sp;
	unsigned int cpsr;
	unsigned int status;
	unsigned int priority;
	unsigned int timestamp;
}

struct thread_ctrl{
	struct thread threads[TOTAL_THREADSIZE];
	unsigned int runs;
	unsigned int next_empty;
	unsigned int next_active;
	unsigned int next_sleeping;
	unsigned int last_active;
	unsigned int idle;
}

struct thread_ctrl thread_queue;

int initThreadQueue( void )
{
	for(int i = 0;i<TOTAL_THREADSIZE;i++){
		thread_queue.next_empty = (int)thread_queue.threads[0];
		thread_queue.runs = -1;
		thread_queue.next_active = 0;
		thread_queue.last_active = 0;
		thread_queue.next_sleeping = 0;
		if( i < TOTAL_THREADSIZE-1 && i != 0 ){
			thread_queue.threads[i].next = thread_queue[i+1];
			thread_queue.threads[i].prev = thread_queue[i-1];
		}
		else if( i == 0){
			thread_queue.threads[i].next = thread_queue[i+1];
			thread_queue.threads[i].prev = next_empty;	
		}
		else{
			thread_queue.threads[i].next = 0;
			thread_queue.threads[i].prev = thread_queue[i-1];
		}
	}
}

int initThread( int pos )
{
	unsigned int prev;
	unsigned int next;
	for(int i = 0;i<12;i++){
		thread_queue.threads[pos].reg[i] = 0;
	}
	unsigned int sp;
	unsigned int cpsr;
	unsigned int status;
	unsigned int priority;
	unsigned int timestamp;	
}

int initConsolenThread( void )
{
	
}

void nextThread( void )
{
	if( !curr_active || !curr_sleeping )
		initConsolenThread();

}

void changeToThread( unsigned int nr )
{
	
}

void saveContext( void )
{
	
}

void changeFromSys2Irq( void )
{
	
}

void changeFromSys2Svc( void )
{
	
}

void changeFromIrq2Sys( void )
{
	
}

void changeFromSvc2Sys( void )
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