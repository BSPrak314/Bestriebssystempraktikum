
#ifndef _reg_operations_asm_h_
#define _reg_operations_asm_h_

void 		asm_CPSR_enableIRQ( void );
void 		asm_CPSR_disableIRQ( void );
void 		asm_CPSR_enableFIQ( void );
void 		asm_CPSR_disableFIQ( void );
unsigned int asm_getCPSR( void );
unsigned int asm_getSPSRforNewThread( void );
unsigned int asm_getRegisters( void );
unsigned int asm_saveSysRegisterToTmp( void );
void 		asm_loadSysRegisterFromTmp( unsigned int [] );
unsigned int	asm_getProcessorMode( void );

#endif