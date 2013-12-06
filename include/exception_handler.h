
#ifndef _exception_handler_H_
#define _exception_handler_H_

struct reg_info{
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;
	unsigned int lr;
};

void init_IVT( void );
void print_reginfo( struct reg_info*);
void print_cpsr( void );
void print_register( unsigned int );
void print_allRegisters( void );
int handle_reset( void );
int handle_undef_instr(struct reg_info*);
int handle_swi(struct reg_info*);
int handle_prefetch_abort(struct reg_info*);
int handle_data_abort(struct reg_info*);
int handle_irq( void );
int handle_fiq(struct reg_info *);
int handle_spurious( void );

#endif
