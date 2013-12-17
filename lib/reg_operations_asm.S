
.global asm_getCPSR
asm_getCPSR:
        mrs      r0, cpsr                       // load status register
        mov      pc, lr                         // jump back

.global asm_getSPSRforNewThread
asm_getSPSRforNewThread:
        mrs      r0, cpsr                       // load status register
        orr      r0, r0, #0b11111                  // set mode bits to system mode ; logical ORR
        //bic      r0, #0xf0000000                // now clear 4 msbits, the N-,Z-,C-,V- bit ; BitClear
        bic      r0, r0, #0b110000000                  // enable IRQ by clearing 7th bit
        mov      pc, lr                         // jump back

.global asm_getRegisters
asm_getRegisters:
        mov     r0, sp                          // parameter to other registers on stack
        mov     pc, lr                          // jump back

.global asm_getProcessorMode
asm_getProcessorMode:
        mrs     r0, cpsr                        // get cpsr
        bic     r0, r0, #0xFFFFFFE0             // clear everyting except status bits
        mov     pc, lr                          // jump back

.global asm_setModeToUNDEFINED
asm_setModeToUNDEFINED:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        bic     r0, r0, #0b11111                // clear last 5 bits      
        orr     r0, r0, #0b11011                // set mode to undefined mode (UND)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_setModeToABORT
asm_setModeToABORT:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        bic     r0, r0, #0b11111                // clear last 5 bits      
        orr     r0, r0, #0b10111                // set mode to abort mode (ABT)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_setModeToIRQ
asm_setModeToIRQ:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        bic     r0, r0, #0b11111                // clear last 5 bits      
        orr     r0, r0, #0b10010                // set mode to interrupt request mode (irq)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_setModeToFIQ
asm_setModeToFIQ:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        bic     r0, r0, #0b11111                // clear last 5 bits      
        orr     r0, r0, #0b10001                // set mode to fast interrupt mode = 10001 (fiq)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_setModeToSYS
asm_setModeToSYS:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        orr     r0, r0, #0b11111                // set mode to system mode (sys)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_setModeToSVC
asm_setModeToSVC:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        bic     r0, r0, #0b11111                // clear last 5 bits      
        orr     r0, r0, #0b10011                // set mode to supervisor mode (svc)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_setModeToUSER
asm_setModeToUSER:
        mrs     r0, cpsr                        // test if we are in user mode
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        tst     r0, #0x0                        // compare with 0, update condition flag
        moveq   pc, lr                          // if true, jump back
        mrs     r0, cpsr                        // else: get cpsr
        bic     r0, r0, #0b1111                 // clear last 4 bits this sets mode to supervisor mode (svc)
        msr     cpsr, r0                        // write cpsr with new mode
        mov     pc, lr                          // jump back

.global asm_isMode_USER
asm_isMode_USER:
        mrs     r0, cpsr                        // get cpsr
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        ands    r0, r0, #0x0                    // compare with 0, update condition flag, and write result in r0
        mov     pc, lr                          // jump back

.global asm_isMode_IRQ
asm_isMode_IRQ:
        mrs     r0, cpsr                        // get cpsr
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        ands    r0, r0, #0b0010                 // compare with 0, update condition flag, and write result in r0
        mov     pc, lr                          // jump back

.global asm_isMode_SVC
asm_isMode_SVC:
        mrs     r0, cpsr                        // get cpsr
        bic     r0, #0xFFFFFFF0                 // clear everything except last 4 bits
        ands    r0, r0, #0b0011                 // compare with 0, update condition flag, and write result in r0
        mov     pc, lr                          // jump back