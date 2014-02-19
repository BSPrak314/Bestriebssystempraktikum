
#include <mem_ctrl.h>
#include <interrupt_handler.h>
#include <led.h>
#include <interrupt_handler_asm.h>
#include <aic.h>
#include <sys_timer.h>
#include <dbgu.h>
#include <thread.h>

//#include <shell.h>
#include <printf.h>
#include <syscall.h>
#include <application.h>

void startOS(void)
{
	
	/* stacks where initalized before in start.S 
     	* now remapping memory, function of mem_ctrl.c */
	//mc_remapMemory();

	// now initalizing IVT, function of interrupt_handler.c
	init_IVT();

	red_on();

	// now initalizing AIC, with some function of aic.c 
	// write pointer to interrupt handlers in corresponding registers of aic
	// ... even if its not necessary, while we use our own IVT
	// enable Interrupt Line No.1 - corresponding to IRQ interrupts
	aic_setInterruptVector_nr(1,(unsigned int)asm_handle_irq );
	aic_setSpuriousVector( (unsigned int)handle_spurious );
	aic_enableInterrupt_nr(1);

	yellow_on();
	
	// now initalizing AIC, with some function of sys_timer.c 
	// set Periodic TimeIntervall to 1sec and enable PeriodicIntervallTimer 
	// then sets RealTimeValue to about 32.77 microsec
	st_setPeriodicValue(0x00001000);
	st_enablePIT();
	st_setRealTimeValue(0x00000001);
	
	// now initalizing IO Buffers in dbgu and makes sure that reading/writing is enabled
	dbgu_start();

	mc_initMMU();

	// now initalizing Thread Control Structur
	thread_initThreadControl();

	green_on();
	
	// enables IRQ Interrupts by clearing the 7th bit in CPSR
	asm_CPSR_enableIRQ();

	// Kernel init DONE - switch off stage leds
	red_off();
	yellow_off();
	green_off();

	syscall_newProcess((void*)&app_run_app7,(void*)0);

}