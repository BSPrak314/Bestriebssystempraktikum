
.global mmu_enable
mmu_enable:
	mrc p15,0,r0,c1,c0,0
	orr r0, #0x1
	mcr p15,0,r0,c1,c0,0
	mov pc, lr

.global mmu_highExceptionVectors_enabled
mmu_highExceptionVectors_enabled:
	mrc p15,0,r0,c1,c0,0
	and r0, r0, #0x2000 	// 13 bit <=> HighExceptions on
	mov pc, lr	

.global mmu_enable_andCache
mmu_enable_andCache:
	mrc p15,0,r0,c1,c0,0
	orr r0, #0x1000		// 12 bit <=> ICache on
	orr r0, #0x4		//  2 bit <=> DCache on
	orr r0, #0x1 		//  1 bit <=> enable MMU on
	mcr p15,0,r0,c1,c0,0
	mov pc, lr

.global mmu_setHighExceptions_on
mmu_setHighExceptions_on:
	mrc p15,0,r0,c1,c0,0
	orr r0, #0x2000		// 13 bit <=> HighExceptionVectors on
	mcr p15,0,r0,c1,c0,0
	mov pc, lr

.global mmu_asm_write_ControlRegister
mmu_asm_write_ControlRegister:
	mcr p15, 0, r0, c1, c0, 0
	mov pc, lr

.global mmu_asm_write_TableAddress
mmu_asm_write_TableAddress:
	mcr p15, 0, r0, c2, c0, 0
	mov pc, lr

.global mmu_asm_write_DomainRegister
mmu_asm_write_DomainRegister:
	mcr p15, 0, r0, c3, c0, 0
	mov pc, lr

.global mmu_asm_read_ControlRegister
mmu_asm_read_ControlRegister:
	mrc p15, 0, r0, c1, c0, 0
	mov pc, lr

.global mmu_asm_read_FaultStatusRegister
mmu_asm_read_FaultStatusRegister:
	mrc p15, 0, r0, c5, c0, 0
	mov pc, lr

.global mmu_asm_read_FaultAddressRegister
mmu_asm_read_FaultAddressRegister:
	mrc p15, 0, r0, c6, c0, 0
	mov pc, lr

.global mmu_asm_invalidate_ICache
mmu_asm_invalidate_ICache:
	mcr p15, 0, r0, c7, c5, 0
	mov pc, lr

.global mmu_asm_invalidate_DCache
mmu_asm_invalidate_DCache:
	mcr p15, 0, r0, c7, c6, 0
	mov pc, lr

.global mmu_asm_invalidate_I_TLB
mmu_asm_invalidate_I_TLB:
	mcr p15, 0, r0, c8, c5, 0
	mov pc, lr

.global mmu_asm_invalidate_D_TLB
mmu_asm_invalidate_D_TLB:
	mcr p15, 0, r0, c8, c6, 0
	mov pc, lr

.global mmu_asm_writeFCSE
mmu_asm_writeFCSE:
	mcr p15, 0, r0, c13, c0, 0
	mov pc, lr

.global mmu_asm_readFCSE
mmu_asm_readFCSE:
	mrc p15, 0, r0, c13, c0, 0
	mov pc, lr