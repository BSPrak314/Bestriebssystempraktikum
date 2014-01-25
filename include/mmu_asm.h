
void mmu_enable( void );
void mmu_enable_andCache( void );
void mmu_setHighExceptions_on( void );
unsigned int mmu_highExceptionVectors_enabled( void );
void mmu_asm_write_ControlRegister(unsigned int);
void mmu_asm_write_TableAddress( unsigned int);
void mmu_asm_write_DomainRegister(unsigned int);
unsigned int mmu_asm_read_ControlRegister(void);
unsigned int mmu_asm_read_FaultStatusRegister(void);
unsigned int mmu_asm_read_FaultAddressRegister(void);
void mmu_asm_invalidate_DCache(void);
void mmu_asm_invalidate_ICache(void);
void mmu_asm_invalidate_I_TLB(void);
void mmu_asm_invalidate_D_TLB(void);