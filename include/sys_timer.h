

#ifndef _sys_timer_H_
#define _sys_timer_H_

#include <thread.h>

void    st_resetWatchdog( void );
void    st_enableWatchdogReset( void );
void    st_setWatchdogValue( unsigned int );
void    st_setPeriodicValue( unsigned int );
void    st_setRealTimeValue( unsigned int );
void    st_setAlarmValue( unsigned int );
int     st_setAlarm( unsigned int );
void    st_initPeriodicValue( void );
void    st_initRealtimeValue( void );
void    st_enablePIT( void );
void    st_enableWDT( void );
void    st_enableRTT( void );
void    st_enableAlarmInterrupt( void );
void    st_disablePIT( void );
void    st_disableWDT( void );
void    st_disableRTT( void );
void    st_disableAlarmInterrupt( void );
int     st_dealWithInterrupts( struct registerStruct * );
void 	st_handlePIT( struct registerStruct * );
void 	st_handleAlarmInterrupt( void );
int 	st_getStatusRegister( void );
int 	st_getTimeStamp(void) ;
int     st_triggeredPIT( unsigned int );
int     st_triggeredWDT( unsigned int );
int     st_triggeredRTT( unsigned int );
int     st_triggeredAlarmInterrupt( unsigned int );
int     st_withPIT( void );
int     st_withWDT( void );
int     st_withRTT( void );
int     st_withAlarmInterrupt( void );

#endif