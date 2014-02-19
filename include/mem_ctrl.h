

#ifndef _mem_ctrl_H_
#define _mem_ctrl_H_

void 		mc_remapMemory( void) ;
unsigned int 	mc_allocMemory( unsigned int, unsigned int, unsigned int );
unsigned int 	mc_getAbortAdress( void );
unsigned int 	mc_getMVA( void );
unsigned int 	mc_getAbortType( void );
unsigned int    mc_getAbortStatus(void );
unsigned int 	mc_init_L1_Table( void );
unsigned int 	mc_init_L2_Tables( void );
unsigned int 	mc_initMMU( void );
unsigned int    mc_userStacks_l2table_enabled( void );
void 		mc_switch_userStack(unsigned int, unsigned int );
void 		mc_fastContextSwitch(unsigned int);
void 		mc_disableStack_forThread(unsigned int, unsigned int );
void 		mc_enableStack_forThread(unsigned int, unsigned int );

#endif