
#ifndef _systemtests_H_
#define _systemtests_H_

void systest_provoke_data_abort(void);
void systest_provoke_sw_inter(void);
void systest_provoke_undef_inst(void);
void systest_testBufferedIO(void);
void systest_threadTest( void );
void systest_dummyThread( void );

#endif