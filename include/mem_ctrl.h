

#ifndef _mem_ctrl_H_
#define _mem_ctrl_H_

void 			mc_remapMemory( void) ;
unsigned int 	mc_isUndefAdress( void );
unsigned int 	mc_isMisalignment( void );
unsigned int 	mc_getAbortAdress( void );
unsigned int 	mc_getAbortType( void );

#endif