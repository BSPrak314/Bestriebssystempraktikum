
#ifndef _exception_handler_H_
#define _exception_handler_H_

#define SWI_KILL 	0xFFFFFF
#define SWI_SLEEP 	0xF00000
#define SWI_WAIT_TIME   0xF00001
#define SWI_WAIT_INPUT  0xF00002
#define SWI_WAIT_OUTPUT 0xF00003
#define SWI_WAIT_IO	0xF00004
#define SWI_WAIT_CODE	0xF00005
#define SWI_WAKEUP 	0x00000F

#define CALL_KILL_SWI 		"SWI 0xFFFFFF"
#define CALL_SLEEP_SWI		"SWI 0xF00000"
#define CALL_WAIT_TIME_SWI 	"SWI 0xF00001"
#define CALL_WAIT_INPUT_SWI 	"SWI 0xF00002"
#define CALL_WAIT_OUTPUT_SWI 	"SWI 0xF00003"
#define CALL_WAIT_IO_SWI 	"SWI 0xF00004"
#define CALL_WAIT_CODE_SWI 	"SWI 0xF00005"
#define CALL_WAKEUP_SWI 	"SWI 0x00000F"

struct reg_info{
	unsigned int r[13];
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
