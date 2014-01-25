
#ifndef _exception_handler_H_
#define _exception_handler_H_

#include <thread.h>

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
