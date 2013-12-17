  // Jump funcions to the interrupt handler functions at "interrupt_handler.c"
.section .text

.global asm_handle_irq
asm_handle_irq:
        sub     lr, lr, #4
        stmfd sp!, {r0-r12,lr}                  // save register

        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // set mode to sys (system mode = 11111)
        msr      cpsr, r0                       // set status register       
        mov      r1, sp
        mov      r2, lr
        msr      cpsr, r12                      // set status register
        mrs      r3, spsr
        stmfd    sp!, {r1-r3}
        mov      r0, sp
        
        bl handle_irq                           // execute c-handler

        ldmfd sp!, {r1-r3}
        msr      spsr, r3
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // set mode to sys (system mode = 11111)
        msr      cpsr, r0                       // set status register       
        mov      sp, r1
        mov      lr, r2
        msr      cpsr, r12                      // set status register
        ldmfd sp!, {r0-r12,lr}                  // write back register
        movs pc, lr

.global asm_handle_reset
asm_handle_reset:
        sub     lr, lr, #0x4                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        bl handle_reset                         // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        adds pc, r14, #0x4                      // jump back

.global asm_handle_undef_inst
asm_handle_undef_inst:
        sub     lr, lr, #0x4                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mov r0, sp                              // parameter to print out register information
        bl handle_undef_inst                    // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        adds pc, r14, #0x4                      // jump back

.global asm_handle_swi
asm_handle_swi:
        stmfd    sp!, {r0-r12,lr}               // save register
        
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // set mode to sys (system mode = 11111)
        msr      cpsr, r0                       // set status register       
        mov      r1, sp
        mov      r2, lr
        msr      cpsr, r12                      // set status register
        mrs      r3, spsr
        stmfd    sp!, {r1-r3}
        mov      r0, sp
        
        bl handle_swi                           // execute c-handler

        ldmfd    sp!, {r1-r3}

/*        push {r0-r3,r12,lr}
        ldr  r0, =string2
        mov r1, r2
        bl print
        pop {r0-r3,r12,lr} */

        msr      spsr, r3
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // set mode to sys (system mode = 11111)
        msr      cpsr, r0                       // set status register       
        mov      sp, r1
        mov      lr, r2
        msr      cpsr, r12                      // set status register
        ldmfd sp!, {r0-r12,lr}                  // write back register

/*      push {r0-r3,r12,lr}
        ldr  r0, =string
        mov r1, lr
        bl print
        pop {r0-r3,r12,lr} */
        movs pc, lr                             // jump back
        //string2: .asciz "JumpinX to %x \n"
        //string: .asciz "Jumping to %x \n"


.global asm_handle_prefetch
asm_handle_prefetch:
        sub     lr, lr, #0x4                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mov r0, sp                              // parameter to print out register information
        bl handle_prefetch                      // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        adds pc, r14, #0x4                      // jump back

.global asm_handle_data_abort
asm_handle_data_abort:
        sub     lr, lr, #0x8                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mov r0, sp                              // parameter to print out register information
        bl handle_data_abort                    // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        adds pc, r14, #0x4                      // jump back

.global asm_handle_fiq
asm_handle_fiq:
        sub     lr, lr, #0x4                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mov r0, sp                              // parameter to print out register information
        bl handle_fiq                           // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        movs pc, lr

.global asm_handle_spurious
asm_handle_spurious:
        stmfd sp!, {r0-r12,lr}                  // save register
        bl handle_spurious                      // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        subs pc, lr, #0x4                       // jump back

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
