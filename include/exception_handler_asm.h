
#ifndef _exception_handler_asm_h_
#define _exception_handler_asm_h_

void 		asm_handle_reset( void );
void 		asm_handle_undef_inst( void );
void 		asm_handle_swi( void );
void 		asm_handle_prefetch( void );
void 		asm_handle_data_abort( void );
void 		asm_handle_irq( void );
void 		asm_handle_fiq( void );
void 		asm_handle_spurious( void );
void 		asm_CPSR_enableIRQ( void );
void 		asm_CPSR_disableIRQ( void );
void 		asm_CPSR_enableFIQ( void );
void 		asm_CPSR_disableFIQ( void );
unsigned int asm_getCPSR( void );
unsigned int asm_getLR( void );
unsigned int asm_getRegisters( void );
unsigned int asm_sysRegisterFromStack( void );
void 		asm_sysRegisterToStack( unsigned int [] );

#endif