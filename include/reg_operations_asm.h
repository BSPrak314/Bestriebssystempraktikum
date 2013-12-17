
#ifndef _reg_operations_asm_h_
#define _reg_operations_asm_h_

#include <thread.h>

unsigned int 	asm_getCPSR( void );
unsigned int 	asm_getSPSRforNewThread( void );
unsigned int *	asm_getRegisters( void );
unsigned int	asm_getProcessorMode( void );
void 		asm_setModeToUNDEFINED( void );
void 		asm_setModeToABORT( void );
void 		asm_setModeToIRQ( void );
void 		asm_setModeToFIQ( void );
void 		asm_setModeToSYS( void );
void		asm_setModeToSVC (void );
void 		asm_setModeToUSER( void );
int 		asm_isMode_USER( void );
int 		asm_isMode_IRQ( void );
int 		asm_isMode_SVC( void );

#endif