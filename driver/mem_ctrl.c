
/* address DEBUG UNIT*/
#define MC 0xFFFFFF00

/* Control Bits */
#define RCB      (1)		/* Remap Command Bit, 1 = remap          */

/* Status Bits */
#define UNDADD   (1)		/* 1 = abort due access of an undefined address in address space */
#define MISADD   (1 << 1)	/* 1 = abort due address misalignment	 */
#define ABTSZ_1  (1 << 7)	/* 0 = Byte|Word; 1 = Half-word|Reserved */
#define ABTSZ_2  (1 << 8)	/* 0 = Byte|Half-word; 1 = Word|Reserved */
#define ABTTYP_1 (1 << 9)	/* 0 = DataRead|CodeFetch; 1 = DataWrite|Reserved */
#define ABTTYP_2 (1 << 10)	/* 0 = DataRead|DataWrite; 1 = CodeFetch|Reserved */
#define MST0 	(1 << 15)	/* 1 = abort due ARM920T 				 */
#define MST1 	(1 << 16)	/* 1 = abort due PDC 					 */
#define MST2 	(1 << 17)	/* 1 = abort due UHP 					 */
#define MST3 	(1 << 18)	/* 1 = abort due EMAC 					 */
#define SVMST0  (1 << 23)	/* 1 = at least one abort due ARM920T since last read of MC_ASR */
#define SVMST1  (1 << 24)	/* 1 = at least one abort due PDC since last read of MC_ASR */
#define SVMST2  (1 << 25)	/* 1 = at least one abort due UHP since last read of MC_ASR */
#define SVMST3  (1 << 26)	/* 1 = at least one abort due EMAC since last read of MC_ASR */

/* in case of equal priority master0 highest, master3 lowest priority */
#define MSTP00  (1)	  		/* ARM920T Priority	LSB	 			     */
#define MSTP01  (1 << 1)	/* ARM920T Priority                      */
#define MSTP02  (1 << 2)	/* ARM920T Priority MSB                  */
#define MSTP10  (1 << 3)	/* PDC Priority LSB				 		 */
#define MSTP11  (1 << 4)	/* PDC Priority 				 		 */
#define MSTP12  (1 << 5)	/* PDC Priority MSB				 		 */
#define MSTP20  (1 << 7)	/* UHP Priority LSB				 		 */
#define MSTP21  (1 << 8)	/* UHP Priority 				 		 */
#define MSTP22  (1 << 9)	/* UHP Priority MSB				 		 */
#define MSTP30  (1 << 11)	/* EMAC Priority LSB			 		 */
#define MSTP31  (1 << 12)	/* EMAC Priority 				 		 */
#define MSTP32  (1 << 13)	/* EMAC Priority MSB			 		 */

/*
 * status bits also where used by memory controler
 * MC_RCR to remap memory from 00200000 to 00000000
 * MC_ASR to check Abort Status
 * MC_AASR to check Abort Adress Status
 */

struct mc_interface {
	unsigned int MC_RCR;		/* Write-only */
	unsigned int MC_ASR;		/* Read-only */
	unsigned int MC_AASR;		/* Read-only */
	unsigned int MC_MPR;		/* Read/Write*/
};

static volatile
struct mc_interface * const mem_ctrl = (struct mc_interface *)MC;

void mc_remapMemory(void)
{
	mem_ctrl->MC_RCR = (unsigned int)1;
}

unsigned int mc_isUndefAdress(void)
{
	return (mem_ctrl->MC_ASR & UNDADD);
}

unsigned int mc_isMisalignment(void)
{
	return (mem_ctrl->MC_ASR & MISADD);
}

unsigned int mc_getAbortAdress(void)
{
	return (mem_ctrl->MC_AASR);
}

unsigned int mc_getAbortType(void)
{
	if(mem_ctrl->MC_ASR & ABTTYP_1 ){
		if(mem_ctrl->MC_ASR & ABTTYP_2 )
			return 3;
		else
			return 1;
	}else{
		if(mem_ctrl->MC_ASR & ABTTYP_2 )
			return 2;
		else
			return 0;
	}
	return -1;
}
