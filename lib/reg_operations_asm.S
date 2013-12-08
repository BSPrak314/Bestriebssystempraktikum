.global asm_CPSR_enableIRQ
asm_CPSR_enableIRQ:
        mrs      r0, cpsr                       // load status register
        bic      r0, r0, #0x80                  // enable IRQ
        msr      cpsr, r0                       // set status register
        mov      pc, lr                         // jump back

.global asm_CPSR_disableIRQ
asm_CPSR_disableIRQ:
        mrs      r0, cpsr                       // load status register
        orr      r0, r0, #0x80                  // disable IRQ
        msr      cpsr, r0                       // set status register
        mov      pc, lr                         // jump back

.global asm_CPSR_enableFIQ
asm_CPSR_enableFIQ:
        mrs      R0, cpsr                       // load status register
        bic      r0, r0, #0x40                  // enable IRQ
        msr      cpsr, r0                       // set status register
        mov      pc, lr                         // jump back

.global asm_CPSR_disableFIQ
asm_CPSR_disableFIQ:
        mrs      r0, cpsr                       // load status register
        orr      r0, r0, #0x40                  // disable IRQ
        msr      cpsr, r0                       // set status register
        mov      pc, lr                         // jump back

.global asm_getCPSR
asm_getCPSR:
        mrs      r0, cpsr                       // load status register
        mov      pc, lr                         // jump back

.global asm_getSPSRforNewThread
asm_getSPSRforNewThread:
        mrs      r0, cpsr                       // load status register
        orr      r0, r0, #0x1F                  // set mode bits to system mode ; logical ORR
        bic      r0, #0xf0000000                // now clear 4 msbits, the N-,Z-,C-,V- bit ; BitClear
        bic      r0, r0, #0x80                  // enable IRQ by clearing 7th bit
        mov      pc, lr                         // jump back

.global asm_getRegisters
asm_getRegisters:
        mov     r0, sp                          // parameter to other registers on stack
        mov     pc, lr                          // jump back

.global asm_saveSysRegisterToTmp
asm_saveSysRegisterToTmp:
        stmfd   sp, {sp,lr}^                    // save usr/sys sp,lr register
        sub     sp, sp, #0xC;
        mrs     sp, spsr                        // save spsr_irq-register (cpsr from usr/sys)
        add     r0, sp, #0x40                   // set r0 to sp position where r0-r12 from usr/sys mode is saved
        bl saveRegisterInArray
        add     sp, sp, #0xC                    // set sp to value before saving
        mov     pc, lr                          // jump back

.global asm_loadSysRegisterFromTmp
asm_loadSysRegisterFromTmp:
        add     sp, sp, #0x34                   // r0 now points to registers on stack
        ldr     r1, =0xd                        // our loop variable
        loop:
            str     r0, [sp]                    // replace old saved r[i] on stack with value from param
            sub     sp, sp, #0x4                // go down on the stack
            add     r0, r0, #0x4                // get next register value 
            sub     r1, r1, #0x1                // loop ariable
            tst     r1, #0x0                    // set comparison flag; test if r1 == 0
        blne loop                               // execute branch if compare flag = 0
        stm     r0, {sp}^                       // write to usr/sy_sp
        add     r0, r0, #0x4
        stm     r0, {lr}^                       // write usr/sys_lr
        add     r0, r0, #0x4
        msr     spsr, r0                        // write usr/sys_cpsr to spsr_irq
        mov     pc, lr                          // jump back

.global asm_getProcessorMode
asm_getProcessorMode:
        mrs r0, cpsr                            // get cpsr
        bic r0, r0, #0xFFFFFFE0                 // clear everyting except status bits
        mov pc, lr                              // jump back
