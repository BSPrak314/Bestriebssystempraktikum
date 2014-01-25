
#define AIC 0xfffff000

#define PRIOR0          (1)
#define PRIOR1          (1 << 1)
#define PRIOR2          (1 << 2)
#define SRCTYPE0        (1 << 4)
#define SRCTYPE1        (1 << 5)


struct  aic_interface {
        unsigned int AIC_SMR[32];       /* Source Mode   Reg 0-31         : Read/Write */
        unsigned int AIC_SVR[32];       /* Source Vector Reg 0-31         : Read/Write */
        unsigned int AIC_IVR;           /* Interrupt Vector Reg           : Read-only  */
        unsigned int AIC_FVR;           /* F-Interrupt Vector Reg         : Read-only  */
        unsigned int AIC_ISR;           /* Interrupt Status Reg           : Read-only  */
        unsigned int AIC_IPR;           /* Interrupt Pending Reg          : Read-only  */
        unsigned int AIC_IMR;           /* Interrupt Mask Reg             : Read-only  */
        unsigned int AIC_CISR;          /* Core Interrupt Status Reg      : Read-only  */ 
        unsigned int reserved[2];
        unsigned int AIC_IECR;          /* Interrupt Enable  Command Reg  : Write-only */
        unsigned int AIC_IDCR;          /* Interrupt Disable Command Reg  : Write-only */
        unsigned int AIC_ICCR;          /* Interrupt Clear   Command Reg  : Write-only */
        unsigned int AIC_ISCR;          /* Interrupt Set     Command Reg  : Write-only */
        unsigned int AIC_EOICR;         /* End of            Command Reg  : Write-only */
        unsigned int AIC_SPU;           /* Spurious Interrupt Vector Reg  : Read/Write */
        unsigned int AIC_DCR;           /* Debug Control Reg              : Read/Write */
};

/* actual struct to manage the address sensitive  registers of the aic*/
static volatile
struct aic_interface * const aic = (struct aic_interface *)AIC;

int aic_readIVR_nr(unsigned int reg)
{
        if(reg > 31)
              return -1;
        return aic->AIC_IVR & (1 << reg);
}

/* set pointer to spurious interrupt handler to corresponding address */
void aic_setSpuriousVector(unsigned int addr)
{
        aic->AIC_SPU = addr;
}

/* set pointer to interrupt handler to corresponding address */
void aic_setInterruptVector_nr(unsigned int reg, unsigned int addr)
{
        if(reg > 31)
              return;        
        aic->AIC_SVR[reg] = addr;
}

unsigned int aic_statusInterrupt_nr(unsigned int reg)
{
        if(reg > 31)
              return -1;
        return aic->AIC_ISR & (1 << reg);
}

unsigned int aic_pendingInterrupt_nr(unsigned int reg)
{
        if(reg > 31)
              return -1;
        return aic->AIC_IPR & (1 << reg);
}

/* enable corresponding interrupt on aic side */
void aic_enableInterrupt_nr(unsigned int reg){
        aic->AIC_IECR = (1 << (reg));
}

/* disable corresponding interrupt on aic side */
void aic_disableInterrupt_nr(unsigned int reg){
        aic->AIC_IDCR = (1 << (reg));
}

void aic_clearInterrupt_nr(unsigned int reg){
        aic->AIC_ICCR = (1 << (reg));
}

int aic_isLineActive_nr(unsigned int reg)
{ 
      if(reg > 31)
              return -1;
      return aic->AIC_CISR & (1 << reg);
}

void aic_setInterrupt_nr(unsigned int reg){
        aic->AIC_ISCR = (1 << (reg));
}

/* signals aic interrupt handling is done */
void aic_endOfInterrupt(void){
        aic->AIC_EOICR = (1 << 1);
}