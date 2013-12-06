
#include <printf.h>

/* address SYSTEM TIMER*/
#define ST_BASE 0xFFFFFD00
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
                                                        
struct st {
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

char* infoPIT = 0;

static volatile
struct st * const sys_timer = (struct st *)ST_BASE;

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
        sys_timer->ST_WDMR |= RSTEN;
}

void st_setWatchdogValue(unsigned int counter)
{
        sys_timer->ST_WDMR = counter;
}

void st_setPeriodicValue(unsigned int counter)
{
        sys_timer->ST_PIMR = counter;
}

void st_setRealtimeValue(unsigned int counter)
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

int st_triggeredPIT(void)
{
        return (sys_timer->ST_SR & PITS) && (sys_timer->ST_IMR & PITS);
}

int st_triggeredWDT(void)
{
        return (sys_timer->ST_SR & WDOVF) && (sys_timer->ST_IMR & WDOVF);
}

int st_triggeredRTT(void)
{
        return (sys_timer->ST_SR & RTTINC) && (sys_timer->ST_IMR & RTTINC);
}

int st_triggeredAlarmInterrupt(void)
{
        return (sys_timer->ST_SR & ALMS) && (sys_timer->ST_IMR & ALMS);
}

void st_handlePIT( void )
{
        if(infoPIT != 0)
                printf(infoPIT);
}

int st_dealWithInterrupts( void )
{
        if( st_triggeredPIT() ){
                st_handlePIT();               
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
