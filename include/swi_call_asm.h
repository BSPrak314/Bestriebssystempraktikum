#ifndef _swi_call_asm_H_
#define _swi_call_asm_H_

#define SWI_KILL 	0xFFFFFF
#define SWI_CREATE 	0x00000F
#define SWI_YIELD 	0x000FFF

void asm_swi_call_kill( void );
void asm_swi_call_create( struct thread * );
void asm_swi_call_yield( void );

#endif