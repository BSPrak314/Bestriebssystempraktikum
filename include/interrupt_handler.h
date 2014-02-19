
#ifndef _exception_handler_H_
#define _exception_handler_H_

#include <thread.h>

#define IVT_ADDR        0x00200000  //remapped (writable) area for ivt
#define JUMP_ADDR       0x00200020  
#define LD_PC_PC_OFF18  0xE59FF018  //opcode pc = pc offset: 18

struct reg_info{
	unsigned int r[13];
	unsigned int lr;
};

void init_IVT( void );
int handle_reset( void );
int handle_undef_instr(struct reg_info*);
int handle_swi(struct registerStruct *reg);
int handle_prefetch_abort(struct registerStruct*);
int handle_data_abort(struct registerStruct*);
int handle_irq( struct registerStruct * );
int handle_fiq(struct reg_info *);
int handle_spurious( void );

#endif
