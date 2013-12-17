
#include <printf.h>
#include <thread.h>

/* address SYSTEM TIMER*/
#define ST 0xFFFFFD00
#define initalPIV 0x00008000
#define initalWDV 0x00000000
#define initalRTPRES 0x00001000
/* Control Bits */
#define WDRST      (1)          /* Watchdog Timer Restart, 1 = reload startup value */
/* Status Bits */
#define RSTEN   (1 << 16)       /* enable reset due watchdog        */
/* TIMER VALUES */
// RTPRES       first 16 bits of ST_RTMR -> nr of clocks required to increase real-time timer        */
// PIV          first 16 bits of ST_PIMR -> Period Interval Value   */
// WDV          first 16 bits of ST_WDMR -> Watchdog Counter Value  */

/* Status  Bits for ST_SR  */
/* Control Bits for ST_IER */
/* Control Bits for ST_IDR */
/* Status  Bits for ST_IMR */
#define PITS    (1)                     /* 1 = period interval timer has reached 0 since last read of ST_SR                       */
                                                        /* 1 = enables / disables Period Interval Timer Interrupts in ST_IER / ST_IDR */
                                                        /* Period Interval Timer Interrupts enabled ? ST_IMR                                              */
#define WDOVF   (1 << 1)        /* 1 = watchdog timer has reached 0 since last read of ST_SR                      */
                                                        /* 1 = enables / disables Watchdog Overflow Interrupts in ST_IER / ST_IDR         */
                                                        /* Watchdog Overflow Interrupts enabled ? ST_IMR                                                          */    
#define RTTINC  (1 << 2)        /* 1 = real-time timer has been incremented since last read of ST_SR              */
                                                        /* 1 = enables / disables Real-timer Timer Increment Interval Timer Interrupts in ST_IER / ST_IDR */
                                                        /* Real-timer Timer Interrupts enabled ? ST_IMR                                                           */
#define ALMS    (1 << 3)        /* 1 = Alarm compare has been detected since last read of ST_SR                   */
                                                        /* 1 = enables / disables Alarm Status Interrupts in ST_IER / ST_IDR              */
                                                        /* Alarm Status Interrupt enabled ? ST_IMR                                                                        */
                                                        
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

extern unsigned int thread_sheduler_enabled;

char* infoPIT = 0;

static volatile
struct st_interface * const sys_timer = (struct st_interface *)ST;

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

void st_setPeriodicValue(unsigned int counter)
{
        sys_timer->ST_PIMR = counter;
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

void st_enablePIT(void)
{
        sys_timer->ST_IER = PITS;
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

int st_getTimeStamp(void)
{
        return sys_timer->ST_CRTR;
}

int st_triggeredPIT(unsigned int status_reg)
{
        return (status_reg & PITS) && (sys_timer->ST_IMR & PITS);
}

int st_triggeredWDT(unsigned int status_reg)
{
        return (status_reg & WDOVF) && (sys_timer->ST_IMR & WDOVF);
}

int st_triggeredRTT(unsigned int status_reg)
{
        return (status_reg & RTTINC) && (sys_timer->ST_IMR & RTTINC);
}

int st_triggeredAlarmInterrupt(unsigned int status_reg)
{
        return (status_reg) && (sys_timer->ST_IMR & ALMS);
}

void st_handlePIT( struct registerStruct * regStruct )
{
        if(infoPIT != 0)
                print(infoPIT);
        if(thread_sheduler_enabled)
                thread_runSheduler( regStruct );
}

void st_handleAlarmInterrupt( void )
{
        ;
}

int st_dealWithInterrupts( struct registerStruct * regStruct )
{
        unsigned int status_reg = sys_timer->ST_SR;
        if( st_triggeredPIT(status_reg) ){
                st_handlePIT( regStruct );               
        }
        if( st_triggeredAlarmInterrupt(status_reg) ){
                st_handleAlarmInterrupt();               
        }
        return 1;
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
