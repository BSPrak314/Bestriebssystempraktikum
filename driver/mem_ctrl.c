#include <mmu_asm.h>
#include <thread.h>

/* address DEBUG UNIT*/
#define MC 0xFFFFFF00
#define L1_TABLE_BASE 0x23F00000
#define L2_TABLES_BASE (L1_TABLE_BASE + 16*1024)

#define PERIPHERYBASE 0xFFF00000

#define IVT_BASE      0x00200000

#define L1_TABLE_EMPTY		(0x10)
#define READ_ONLY 		(0b00 << 10)
#define FULL_ACCESS_ALL		(0b11 << 10)
#define SYS_FULLACC_USER_NOACC	(0b01 << 10)
#define SYS_FULLACC_USER_READ	(0b10 << 10)
#define L2_ACC_SYSFULL_NOUSER   (0x55 << 4)
#define L2_ACC_SYSFULL_USERREAD (0xAA << 4)
#define L2_ACC_ALL_FULL		(0xFF << 4)
#define SEGMENT			0b10
#define COARSE_PAGETABLE	0b01
#define SMALL_TABLE 		0b10
#define CACHE 		        (0b1 << 3)
#define BUFFER 			(0b1 << 2)

/* Control Bits */
#define RCB      (1)		/* Remap Command Bit, 1 = remap          */

struct mc_interface {
	unsigned int MC_RCR;		/* Write-only */
	unsigned int MC_ASR;		/* Read-only */
};

struct mc_L1_table {
	unsigned int entry[4096];
};

struct mc_L2_coarse_table {
	unsigned int entry[256];
};

struct mc_L2_tables {
	struct mc_L2_coarse_table thread_stacks;
	struct mc_L2_coarse_table periphery;
};

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
 * system has read access, user no access
 *
 * then giving more access to some tables
 * Read/Write for System, User still no access
 * - System Stacks & Thread Control Structs + Thread Array
 * - Periphery
 *
 * Read/Write for System, Read Access for User 
 * - on User Code
 *
 * Read/Write for System and User
 * - on User Stacks                    */
unsigned int mc_init_L1_Table( void )
{
	l1_table = (struct mc_L1_table * )L1_TABLE_BASE;

	// set R bit (bit 9) to 0 and set S bit (bit 8) to 1 in MMU Control Register 
	// <=> Read only means Read only for System, User no access
	unsigned int mmc_ctrl = mmu_asm_read_ControlRegister();
	mmc_ctrl &= 0xFFFFFDFF;
	mmc_ctrl |= (1 << 8);
	mmu_asm_write_ControlRegister(mmc_ctrl);

	int i = 0;
	unsigned int permission = L1_TABLE_EMPTY;
	permission |= READ_ONLY | SEGMENT;
	for( i = 0;i<0x1000;i++){
		l1_table->entry[i] = (i << 20) + permission;
	}

	// System Periphery	-> sys read/write, user no access !! DISABLE BUFFERING, DISABLE CACHE
 	// Address Space 0xFFE00000 - 0xFFFFFFFF
	permission = L1_TABLE_EMPTY;
	permission |= SYS_FULLACC_USER_NOACC | SEGMENT;
	l1_table->entry[4095] = (4095 << 20) + permission;

	// Kernel Stacks 	-> sys read/write, user no access
	// Address Space 0x20000000 - 0x200FFFFF
	permission = L1_TABLE_EMPTY;
	permission |= SYS_FULLACC_USER_NOACC | SEGMENT;
	l1_table->entry[0x23F] 	= (0x23F << 20) + permission;
	// Thread Control Structs + Thread Array -> sys read/write, user no access
	// Address Space 0x00200000 - 0x002FFFFF
	l1_table->entry[0x2] 	= (0x2 << 20) + permission;

	// user code   		-> sys read/write, user read only
	// Address Space 0x20100000 - 0x201FFFFF
	permission = L1_TABLE_EMPTY;
	permission |= SYS_FULLACC_USER_READ | SEGMENT;
	l1_table->entry[0x201] 	= (0x201 << 20) + permission;

	// user stacks		-> sys read/write, user read/write
	// Address Space 0x23E0 0000 - 0x23EF FFFF
	permission = L1_TABLE_EMPTY;
	permission |= FULL_ACCESS_ALL | SEGMENT;
	l1_table->entry[0x23E] 	= (0x23E << 20) + permission;

	mmu_asm_write_TableAddress(L1_TABLE_BASE);

	return 1;
}

static void mc_createL2_table_periphery(void)
{
	// Periphery is not buffered
	unsigned int permission = L2_ACC_SYSFULL_NOUSER;
	permission |= SMALL_TABLE;
	
	int i = 0;
	for(i = 0;i<0x100;i++){
		l2_tables->periphery.entry[i] = (PERIPHERYBASE + (i << 12) ) | permission ;
	}
	// Mapping Address Space for HighExceptionVectors 0xffff0000
	// to beginning of internal Ram <=> IVT table
	// permission |= CACHE | BUFFER;
	l2_tables->periphery.entry[0xf0] = IVT_BASE | permission;
	
}

static void mc_createL2_table_userStacks(void)
{
	unsigned int permission = L2_ACC_SYSFULL_NOUSER | SMALL_TABLE;
//	permission |= CACHE | BUFFER;
	
	int i = 0;
	for(i = 0;i<0x100;i++){
		l2_tables->thread_stacks.entry[i] = (USERSTACK_BASE + (i << 12) ) | permission;
	}
	permission = L2_ACC_ALL_FULL | SMALL_TABLE;
//	permission |= CACHE | BUFFER;
	
	l2_tables->thread_stacks.entry[MAX_THREADS] = (USERSTACK_BASE + (MAX_THREADS << 12) ) | permission;
}

static unsigned int mc_init_L2_Tables( void )
{
	l2_tables = (struct mc_L2_tables * )L2_TABLES_BASE;

	l1_table->entry[0xfff] = COARSE_PAGETABLE | ( (unsigned int)&(l2_tables->periphery) & 0xFFFFFC00 );;
	mc_createL2_table_periphery();
	
	
	l1_table->entry[USERSTACK_BASE/(1024*1024)] = COARSE_PAGETABLE | ( (unsigned int)&(l2_tables->thread_stacks) & 0xFFFFFC00 );;
	mc_createL2_table_userStacks();
	
	return 1;
}

void mc_switch_userStack(unsigned int old_position, unsigned int new_position)
{
	unsigned int permission = L2_ACC_SYSFULL_NOUSER | SMALL_TABLE;
//	permission |= CACHE | BUFFER;
	l2_tables->thread_stacks.entry[old_position] = ( USERSTACK_BASE + (old_position << 12) ) + permission;
	permission = L2_ACC_ALL_FULL | SMALL_TABLE;
//	permission |= CACHE | BUFFER;
	l2_tables->thread_stacks.entry[new_position] = ( USERSTACK_BASE + (new_position << 12) ) + permission;
}

unsigned int mc_userStacks_l2table_enabled( void )
{
	return (l1_table->entry[0x23E] & 0x00000003);
}

void mc_remap_L1_entry_from_to( unsigned int page, unsigned int newpage, unsigned int permission )
{
	if( page >= 0x1000 ){
		page = page >> 20;
	}
	if( newpage >= 0x1000 ){
		newpage = newpage >> 20;
	}
	permission |= L1_TABLE_EMPTY | SEGMENT;
	l1_table->entry[page] 	= (newpage << 20) + permission;
}

unsigned int mc_initMMU(void)
{
	// init all Domain as Managed Permission Access
	mmu_asm_write_DomainRegister(0x55555555);
	
	// init L1 Table and write Table Address to MMU
	mc_init_L1_Table();
	// init L2 Tables for UserStacks and Periphery
	mc_init_L2_Tables();

	// just to demonstrate an non 1:1 mapping
	mc_remap_L1_entry_from_to(0x4, 0x210, SYS_FULLACC_USER_READ);

	// get MMU Control Register
	unsigned int mmc_ctrl = mmu_asm_read_ControlRegister();
	// disabling Instruction and Data Cache (2 bit and 12 bit)
	mmc_ctrl &= 0xFFFFEFFB;
	// write back MMU Control Register
	mmu_asm_write_ControlRegister(mmc_ctrl);
	
	// mmu_asm_invalidate_DCache();
	// mmu_asm_invalidate_ICache();

	mmu_setHighExceptions_on();

	// enableing MMU with Data- and Instruction Cache
	// mmu_enable_andCache();

	// enableing MMU without Data- and Instruction Cache
	mmu_enable();

	// mmu_asm_invalidate_D_TLB();
	// mmu_asm_invalidate_I_TLB();

	return 1;
}

unsigned int mc_getAbortAdress(void)
{
	return mmu_asm_read_FaultAddressRegister();
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
	char faultStatus = (char)mmu_asm_read_FaultStatusRegister();

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
