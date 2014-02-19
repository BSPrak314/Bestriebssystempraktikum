
#include <printf.h>
#include <thread.h>

/* address SYSTEM TIMER*/
#define ST 0xFFFFFD00
#define initalPIV 0x00008000
#define initalWDV 0x00000000
#define initalRTPRES 0x00000010
/* Control Bits */
#define WDRST      (1)          /* Watchdog Timer Restart, 1 = reload startup value */
/* Status Bits */
#define RSTEN   (1 << 16)       /* enable reset due watchdog        */
/* TIMER VALUES */
// RTPRES       first 16 bits of ST_RTMR -> nr of clocks required to increase real-time timer */
// PIV          first 16 bits of ST_PIMR -> Period Interval Value   */
// WDV          first 16 bits of ST_WDMR -> Watchdog Counter Value  */

/* Status  Bits for ST_SR 
 * Control Bits for ST_IER
 * Control Bits for ST_IDR
 * Status  Bits for ST_IMR */
/* 1 = period interval timer has reached 0 since last read of ST_SR
 * 1 = enables / disables Period Interval Timer Interrupts in ST_IER / ST_IDR
 * Period Interval Timer Interrupts enabled ? ST_IMR                            */
#define PITS    (1)                     
/* 1 = watchdog timer has reached 0 since last read of ST_SR
 * 1 = enables / disables Watchdog Overflow Interrupts in ST_IER / ST_IDR
 * Watchdog Overflow Interrupts enabled ? ST_IMR                                */
#define WDOVF   (1 << 1)        
/* 1 = real-time timer has been incremented since last read of ST_SR
 * 1 = enables / disables Real-timer Timer Increment Interval Timer Interrupts in ST_IER / ST_IDR
 * Real-timer Timer Interrupts enabled ? ST_IMR                                 */
#define RTTINC  (1 << 2)        
/* 1 = Alarm compare has been detected since last read of ST_SR
 * 1 = enables / disables Alarm Status Interrupts in ST_IER / ST_IDR
 * Alarm Status Interrupt enabled ? ST_IMR                                      */
#define ALMS    (1 << 3)        

                                                        
struct st_interface {
        unsigned int ST_CR;             /* Write-only */
        unsigned int ST_PIMR;           /* Read/Write */
        unsigned int ST_WDMR;           /* Read/Write */
        unsigned int ST_RTMR;           /* Read/Write */
        unsigned int ST_SR;             /* Read-only  */
        unsigned int ST_IER;            /* Write-only */
        unsigned int ST_IDR;            /* Write-only */
        unsigned int ST_IMR;            /* Read-only  */
        unsigned int ST_RTAR;           /* Read/Write */
        unsigned int ST_CRTR;           /* Read-only  */
};

/* defined in thread.c 
 * thread_sheduler_enabled == 1 <> st_handlePIT calles thread_runSheduler 
 *                         == 0 <> st_handlePIT does not invoke threads  */
extern unsigned int thread_sheduler_enabled;

/* shared with lib_system/systemtests.c and system/hreads.c 
 * holds infoTag with is printed when an PIT is handled by st_handlePIT  */
// char* infoPIT = 0;

static volatile
struct st_interface * const sys_timer = (struct st_interface *)ST;


void st_enablePIT(void)
{
        sys_timer->ST_IER = PITS;
}

/* handle an PIT 
 * handling depends on two global variables
 * may print an infoTag or call thread_runSheduler to initiate preemtive multitasking  */
void st_handlePIT( struct registerStruct * regStruct )
{
        thread_runSheduler( regStruct, 0 );    
}
/* some systemcalls may enable the AlarmInterrupt, if so look for sleeping threads */
void st_handleAlarmInterrupt(void)
{
        st_enablePIT();
        thread_wakeUp();
}
/* check if PIT is enabled and coresponding bit is active */
int st_triggeredPIT(unsigned int status_reg)
{
        return (status_reg & PITS) && (sys_timer->ST_IMR & PITS);
}
/* check if AlamrInterrupt is enabled and coresponding bit is active */
int st_triggeredAlarmInterrupt(unsigned int status_reg)
{
        return (status_reg & ALMS) && (sys_timer->ST_IMR & ALMS);
}
/* interrupt_handler.c will call this for handle_irq to check for timer interrupts */
int st_dealWithInterrupts( struct registerStruct * regStruct )
{
        unsigned int status_reg = sys_timer->ST_SR;
        if( st_triggeredAlarmInterrupt(status_reg) ){
                st_handleAlarmInterrupt();
        }
        if( st_triggeredPIT(status_reg) ){
                st_handlePIT( regStruct );
        }
        return 1;
}
/* get current RealTimeValue */
int st_getTimeStamp(void)
{
        return sys_timer->ST_CRTR;
}
/* enables AlarmInterrupts, return the wakeUp Time := msec + current RealTimeValue
 * and set the RealTimeAlarmValue to the new wakeUp time, IF there is no Alarm befor */
unsigned int st_setAlarm( unsigned int msec )
{
        sys_timer->ST_IER = ALMS;
        unsigned int currAlarm = sys_timer->ST_RTAR;
        unsigned int currTime = sys_timer->ST_CRTR;
        unsigned int newAlarm = (32768 * msec) / 1000;
        
        newAlarm = newAlarm + currTime;
        if( currAlarm <= currTime ||  currAlarm > newAlarm ){
                sys_timer->ST_RTAR = newAlarm;
        }
        return newAlarm;
}

void st_setAlarmValue( unsigned int value )
{
        sys_timer->ST_RTAR = value;
}

/*
 * some interface functions 
 * to use the system timer in an more human readable form 
 */
void st_initWatchdogValue(void)
{
        sys_timer->ST_WDMR = initalWDV;
}

void st_resetWatchdog(void)
{
        sys_timer->ST_CR = WDRST;
}

void st_enableWatchdogReset(void)
{
        sys_timer->ST_WDMR = RSTEN;
}

void st_setWatchdogValue(unsigned int counter)
{
        sys_timer->ST_WDMR = counter;
}

void st_setPeriodicValue(unsigned int msec)
{
        sys_timer->ST_PIMR = (32768 * msec) / 1000;
}

void st_setRealTimeValue(unsigned int counter)
{
        sys_timer->ST_RTMR = counter;
}

void st_initPeriodicValue(void)
{
        sys_timer->ST_PIMR = initalPIV;
}

void st_initRealtimeValue(void)
{
        sys_timer->ST_RTMR = initalRTPRES;
}

void st_enableWDT(void)
{
        sys_timer->ST_IER = WDOVF;
}

void st_enableRTT(void)
{
        sys_timer->ST_IER = RTTINC;
}

void st_enableAlarmInterrupt(void)
{
        sys_timer->ST_IER = ALMS;
}

void st_disablePIT(void)
{
        sys_timer->ST_IDR = PITS;
}

void st_disableWDT(void)
{
        sys_timer->ST_IDR = WDOVF;
}

void st_disableRTT(void)
{
        sys_timer->ST_IDR = RTTINC;
}

void st_disableAlarmInterrupt(void)
{
        sys_timer->ST_IDR = ALMS;
}

int st_getStatusRegister(void)
{
        return sys_timer->ST_SR;
}

int st_triggeredWDT(unsigned int status_reg)
{
        return (status_reg & WDOVF) && (sys_timer->ST_IMR & WDOVF);
}

int st_triggeredRTT(unsigned int status_reg)
{
        return (status_reg & RTTINC) && (sys_timer->ST_IMR & RTTINC);
}

int st_withPIT(void)
{
        return sys_timer->ST_IMR & PITS;
}

int st_withWDT(void)
{
        return sys_timer->ST_IMR & WDOVF;
}

int st_withRTT(void)
{
        return sys_timer->ST_IMR & RTTINC;
}

int st_withAlarmInterrupt(void)
{
        return sys_timer->ST_IMR & ALMS;
}
