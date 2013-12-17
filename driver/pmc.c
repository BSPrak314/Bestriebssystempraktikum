
/* address SYSTEM TIMER*/
#define PMC 0xFFFFFC00

/* Control Bits */
#define PCK      (1)                       /* ProcessorClock */
                                                        
struct pmc_interface {
        unsigned int PMC_SCER;             /* Write-only SystemClockEnableRegister     */
        unsigned int PMC_SCDR;             /* Write-only SystemClockDisableRegister    */
        unsigned int PMC_SCSR;             /* Read-only  SystemClockStatusRegister     */
        unsigned int reserved0;
        unsigned int PMC_PCER;             /* Write-only PeripheralClockEnableRegister */
        unsigned int PMC_PCDR;             /* Write-only PeripheralClockDisableRegister*/
        unsigned int PMC_PCSR;             /* Read-only  PeripheralClockStatusRegister */
        unsigned int reserved1;    
        unsigned int CKGR_MOR;             /* Read/Write MainOscillatorRegister        */
        unsigned int CKGR_MCFR;            /* Read-only  MainClockFrquencyRegister     */
        unsigned int CKGR_PLLAR;           /* Read/Write PLL A Register                */
        unsigned int CKGR_PLLBR;           /* Read/Write PLL B Register                */
        unsigned int PMC_MCKR;             /* Read/Write MasterClockRegister           */
        unsigned int reserved2[3];
        unsigned int PMC_PCK0;             /* Read/Write ProgrammableClock0 Register   */
        unsigned int PMC_PCK1;             /* Read/Write ProgrammableClock1 Register   */
        unsigned int PMC_PCK2;             /* Read/Write ProgrammableClock2 Register   */
        unsigned int PMC_PCK3;             /* Read/Write ProgrammableClock3 Register   */
        unsigned int reserved3[4];
        unsigned int PMC_IER;              /* Write-only InterruptEnableRegister       */
        unsigned int PMC_IDR;              /* Write-only InterruptDisableRegister      */
        unsigned int PMC_SR;               /* Read-only  StatusRegister                */
        unsigned int PMC_IMR;              /* Read-only  InterruptMaskRegister         */
};

static volatile
struct pmc_interface * const power_ctrl = (struct pmc_interface *)PMC;

void pmc_disableProcessorClock( void )
{
        power_ctrl->PMC_SCDR = PCK ;
}