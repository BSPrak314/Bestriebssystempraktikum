#include <mmu_asm.h>
#include <thread.h>
#include <interrupt_handler.h>

/* address DEBUG UNIT*/
#define MC 			0xFFFFFF00
#define L1_TABLE_BASE 		0x20300000
#define L2_TABLES_BASE 		(L1_TABLE_BASE + 16*1024)

#define FCSE_OFFSET		0x30000000

#define PERIPHERYBASE 		0xFFF00000
#define KERNELBASE 		0x20000000
#define USERBASE 		0x20100000

#define DOMAIN_SYSTEM 	        (0b1111 << 5)

#define CACHE 		        (0b1 << 3)
#define BUFFER 			(0b1 << 2)

#define AP_L1_R__NA 		(0b00 << 10)
#define AP_L1_RW_RW		(0b11 << 10)
#define AP_L1_RW_NA		(0b01 << 10)
#define AP_L1_RW_R		(0b10 << 10)

#define AP_L2_RW_NA 		(0x55 << 4)
#define AP_L2_RW_R 		(0xAA << 4)
#define AP_L2_RW_RW		(0xFF << 4)
#define AP_L2_R__NA 		(0x00 << 4)

#define SECTION 		0b10010
#define COARSE_PAGETABLE	0b10001
#define SMALL_TABLE 		0b10010

/* Control Bits */
#define RCB      (1)		/* Remap Command Bit, 1 = remap          */

struct mc_interface {
	unsigned int MC_RCR;		/* Write-only */
	unsigned int MC_ASR;		/* Read-only */
	unsigned int MC_AASR;		/* Read-only */
};

struct mc_L1_table {
	unsigned int entry[4096];
};

struct mc_L2_coarse_table {
	unsigned int entry[256];
};

struct mc_L2_tables {
	struct mc_L2_coarse_table kernelCode;
	struct mc_L2_coarse_table userCode;
	struct mc_L2_coarse_table periphery;
	struct mc_L2_coarse_table userStacks[MAX_ADD_SPACES];
};

// Linker Script defines end of Kernel/User Code - begin of Kernel/User Data
extern char KERNELDATA;
extern char USERDATA;

static volatile
struct mc_interface * const mem_ctrl = (struct mc_interface *)MC;

static volatile
struct mc_L1_table * l1_table;

static volatile
struct mc_L2_tables * l2_tables;

void mc_remapMemory(void)
{
	mem_ctrl->MC_RCR = (unsigned int)1;
}

/* Starting to restict the access to all memory areas
 * to ALL NO ACCESS
 *
 * then 
 * Read access for system to kernel code at page 0x200
 *
 * Read/Write for System, User still no access
 * all real memory: page 0x0, 0x2, 0x201-0x240, 0xfff
 *
 * Read/Write for System, Read Access for User 
 * - on User Code
 *
 * Read/Write for System and User
 * - on User Stacks                    
 */
unsigned int mc_init_L1_Table( void )
{
	l1_table = (struct mc_L1_table * )L1_TABLE_BASE;
	int i = 0;
	
	// set R bit (bit 9) to 0 and set S bit (bit 8) to 1 in MMU Control Register 
	// <=> Read only means Read only for System, User no access
	unsigned int mmc_ctrl = mmu_asm_read_ControlRegister();
	mmc_ctrl &= 0xFFFFFDFF;
	mmc_ctrl |= (1 << 8);
	mmu_asm_write_ControlRegister(mmc_ctrl);

// step 1 : all memory is not accessable
	unsigned int permission = 0;	
	for( i = 0;i<0x1000;i++){
		l1_table->entry[i] = (i << 20) + permission;
	}

// step 2 : all real memory has managed permission 	
//	page[0x20], page[0x200]-page[0x240],page[0x2],page[0xfff]
//      system has full access - user no access
	permission = SECTION
	        | DOMAIN_SYSTEM 
	        | AP_L1_RW_NA;

	l1_table->entry[0xfff] = (0xfff << 20) + permission;
	l1_table->entry[0x202] = (0x202 << 20) + permission;
	l1_table->entry[0x203] = (0x203 << 20) + permission;
	
// 	enable BUFFER and CACHE for everything 
//	except Periphery and PageTables
	permission |= CACHE | BUFFER;
	l1_table->entry[0x2] = (0x2 << 20) + permission;
	l1_table->entry[0x200] = (0x200 << 20) + permission;
	l1_table->entry[0x201] = (0x201 << 20) + permission;
	for( i = 0x204;i<0x240;i++){
		l1_table->entry[i] = (i << 20) + permission;
	}
	
// step 3 : user code/data page[0x201]  -> sys read/write, user read/write
//	    user stacks	   page[0x23E]	-> sys read/write, user read/write
// 	Address Space 0x20100000 - 0x201FFFFF
	permission = SECTION 
	        | DOMAIN_SYSTEM
	        | AP_L1_RW_RW
	        | BUFFER
	        | CACHE;

	l1_table->entry[0x201] 	= (0x201 << 20) + permission;
	l1_table->entry[0x23E] 	= (0x23E << 20) + permission;

	mmu_asm_write_TableAddress(L1_TABLE_BASE);

	return 1;
}

static void mc_createL2_table(unsigned int * l2_table_address, unsigned int base, unsigned int permission, unsigned int permission2, unsigned int split)
{	
	unsigned int i = 0;
	for(i = 0;i<0x100;i++){
		if( i == split )
			permission = permission2;
		*l2_table_address = (base + (i << 12) ) | permission ;
		l2_table_address++;
	}	
}

static unsigned int mc_init_L2_Tables( void )
{
	unsigned int permission = 0;
	unsigned int permission2 = 0;

	l2_tables = (struct mc_L2_tables * )L2_TABLES_BASE;

	// set entry in L1 table 
	l1_table->entry[0xfff] = COARSE_PAGETABLE 
			| (unsigned int)&(l2_tables->periphery)
			| DOMAIN_SYSTEM;

	permission = AP_L2_RW_NA | SMALL_TABLE;
	
	mc_createL2_table( (unsigned int *)&(l2_tables->periphery), PERIPHERYBASE, permission , permission, 0 );
	// Mapping Address Space for HighExceptionVectors 0xffff0000
	// to beginning of internal Ram <=> IVT table
	l2_tables->periphery.entry[0xf0] = IVT_ADDR | AP_L2_RW_NA | SMALL_TABLE ;

	// Permission for KERNEL Code and KERNEL Data	
	permission  = AP_L2_R__NA | SMALL_TABLE | CACHE | BUFFER;
	permission2 = AP_L2_RW_NA | SMALL_TABLE | CACHE | BUFFER;

	mc_createL2_table( (unsigned int *)&(l2_tables->kernelCode),  KERNELBASE, permission , permission2, KERNELDATA );
	l1_table->entry[0x200] = COARSE_PAGETABLE 
			| (unsigned int)&(l2_tables->kernelCode)
			| DOMAIN_SYSTEM;
	
	// Permission for USER Code and USER Data
	permission  = AP_L2_RW_R  | SMALL_TABLE | BUFFER | CACHE;
	permission2 = AP_L2_RW_RW | SMALL_TABLE | BUFFER | CACHE;

	mc_createL2_table( (unsigned int *)&(l2_tables->userCode), USERBASE, permission , permission2, USERDATA );
	l1_table->entry[0x201] = COARSE_PAGETABLE 
			| (unsigned int)&(l2_tables->userCode)
			| DOMAIN_SYSTEM;

	permission  = AP_L2_RW_NA | SMALL_TABLE | BUFFER | CACHE;
	unsigned int i = 0;
	unsigned int domain = 1;
	for(i= 0;i<MAX_ADD_SPACES;i++){
		unsigned int vaddr = i*0x20;
		vaddr += ( ( FCSE_OFFSET + VIRTUAL_STACK )/0x100000);
		mc_createL2_table( (unsigned int *)&(l2_tables->userStacks[i]), USERSTACK_BASE, permission , permission, 0 );
		l1_table->entry[ vaddr ] = COARSE_PAGETABLE 
			| (unsigned int)&(l2_tables->userStacks[i])
			| (domain << 5);
		domain++;
	}

	return 1;
}

void mc_remap_L1_entry_from_to( unsigned int page, unsigned int newpage, unsigned int permission, unsigned int domain )
{
	if( page >= 0x1000 ){
		page = page >> 20;
	}
	if( newpage >= 0x1000 ){
		newpage = newpage >> 20;
	}
	permission = SECTION | (domain << 5);
	l1_table->entry[page] 	= (newpage << 20) + permission;
}

void mc_enableStack_forThread(unsigned int threadID, unsigned int domain)
{
	unsigned int permission = AP_L2_RW_RW | SMALL_TABLE | BUFFER | CACHE;
	//print("enable for thread %x\ndomain: %x\n",threadID, domain);
	l2_tables->userStacks[domain-1].entry[threadID] = USERSTACK_BASE + permission;
}

void mc_disableStack_forThread(unsigned int threadID, unsigned int domain)
{
	unsigned int permission = AP_L2_RW_NA | SMALL_TABLE | BUFFER | CACHE;
	l2_tables->userStacks[domain-1].entry[threadID] = USERSTACK_BASE + permission;
}

unsigned int mc_allocMemory(unsigned int emptypage, unsigned int domain, unsigned int page_nr)
{
	unsigned int page = ((domain-1)*0x20 ) + (page_nr-1);
	page += (FCSE_OFFSET / 0x100000);
	
	unsigned int permission = SECTION
		| AP_L1_RW_RW
		| BUFFER
		| CACHE;
		

	permission |= (domain << 5);
	emptypage += 0x210;
	l1_table->entry[page] = (emptypage << 20) + permission;
	return 1;
}

void mc_fastContextSwitch(unsigned int domain)
{
	unsigned int registerValue = 0x40000000;
	registerValue |= (01 << (domain*2) );
	//print("current domain register %x\n",registerValue);
	mmu_asm_write_DomainRegister(registerValue);
	domain += (FCSE_OFFSET / 0x2000000) -1;
	mmu_asm_writeFCSE( domain << 25 );
}

unsigned int mc_initMMU(void)
{
	unsigned int mmc_ctrl = mmu_asm_read_ControlRegister();
	// disabling Instruction and Data Cache (2 bit and 12 bit)
	mmc_ctrl &= 0xFFFFEFFB;
	// write back MMU Control Register
	mmu_asm_write_ControlRegister(mmc_ctrl);

	// init all Domain as read only, except System Domain (domain 1111)
	mmu_asm_write_DomainRegister(0x40000000);
	
	// init L1 Table and write Table Address to MMU
	mc_init_L1_Table();
	// init L2 Tables for UserStacks and Periphery
	mc_init_L2_Tables();
	
	mmu_asm_invalidate_DCache();
	mmu_asm_invalidate_ICache();

	mmu_setHighExceptions_on();

	// enableing MMU with Data- and Instruction Cache
	mmu_enable_andCache();
	//mmu_enable();

	mmu_asm_invalidate_D_TLB();
	mmu_asm_invalidate_I_TLB();

	return 1;
}

unsigned int mc_getAbortAdress(void)
{
	return mmu_asm_read_FaultAddressRegister();
}

unsigned int mc_getMVA(void)
{
	return mem_ctrl->MC_AASR;
}

unsigned int mc_getAbortStatus(void)
{
	unsigned int reason = mem_ctrl->MC_ASR & 0x00000C00;
	reason = (reason >> 10);
	switch (reason)
	{
		case 0 :
			// abort during read 
			return 0;
		case 1 :
			// abort during write
			return 1;
		case 2 :
			// abort during instruction fetch
			return 2;
		case 3 :
			// other reasons (reserved) 
			return 3;
		default : break;
	}
	return 4;
}

unsigned int mc_getAbortType(void)
{
	unsigned int faultStatus = mmu_asm_read_FaultStatusRegister();
	faultStatus = (faultStatus << 28);
	faultStatus = (faultStatus >> 28);

	switch( faultStatus ){
	case 0b0001 : ;
	case 0b0011 : {
		// Alignment abort
		return 1;
		}
	case 0b0101 : {
		// Translation abort on Section level
		return 2;
		}
	case 0b0111 : {
		// Translation abort on Page level
		return 3;
		}
	case 0b1001 : {
		// Domain abort on Section level
		return 4;
		}
	case 0b1011 : {
		// Domain abort on Page level
		return 5;
		}
	case 0b1101 : {
		// Permission abort on Page level
		return 6;
		}
	case 0b1111 : {
		// Permission abort on Page level
		return 7;
		}
	case 0b1000 : {
		// Section abort & External abort on noncachable, nonbuffered acccess or nonbufferable read
		return 8;
		}
	case 0b1010 : {
		// Page abort & External abort on noncachable, nonbuffered acccess or nonbufferable read
		return 9;
		}
	}
	
	return 0;
}
