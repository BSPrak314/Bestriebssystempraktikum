

#ifndef _sys_timer_H_
#define _sys_timer_H_

void    st_resetWatchdog( void );
void    st_enableWatchdogReset( void );
void    st_setWatchdogValue( unsigned int );
void    st_setPeriodicValue( unsigned int );
void    st_setRealtimeValue( unsigned int );
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
int     st_dealWithInterrupts( void );
void 	st_handlePIT( void );
int     st_triggeredPIT( void );
int     st_triggeredWDT( void );
int     st_triggeredRTT( void );
int     st_triggeredAlarmInterrupt( void );
int     st_withPIT( void );
int     st_withWDT( void );
int     st_withRTT( void );
int     st_withAlarmInterrupt( void );

#endif