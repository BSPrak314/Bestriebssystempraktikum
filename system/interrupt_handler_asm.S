  // First writes all register values to stack, then 
  // jump funcions to the interrupt handler functions at "interrupt_handler.c"
  // finaly restores register values
.section .text

// two interrupts can contain an context change
// irq interrupts during an PIT or TXRDY Interrupt
// or svc interrupts with an corresponding svc call
// so we need to save r0-r12 + lr_usr,sp_usr,spsr_irq<>cpsr_irq,lr_irq<>pc_usr
.global asm_handle_irq
asm_handle_irq:
        sub      lr, lr, #4                     // adjust lr to next instruction to execute
        stmfd sp!, {r0-r12,lr}                  // save register
        mrs      r12, cpsr                      // load status register to r0
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      r1, sp                         // get sp_usr/sys
        mov      r2, lr                         // get lr_usr/sys
        msr      cpsr, r12                      // back to IRQ
        mrs      r3, spsr                       // get spsr_irq <> cpsr_irq
        stmfd    sp!, {r1-r3}                   // save those values to stack_irq
        mov      r0, sp                         // return pointer to stack_irq
        bl handle_irq                           // execute c-handler
        ldmfd sp!, {r1-r3}                      // everything rewind
        msr      spsr, r3                       // restore spsr
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      sp, r1                         // restore sp_usr/sys
        mov      lr, r2                         // restore lr_usr/sys
        msr      cpsr, r12                      // back to IRQ status register
        ldmfd sp!, {r0-r12,lr}                  // write back register
        movs pc, lr                             // -> S-Bit will restore spsr to cpsr / end of handling

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
        stmfd sp!, {r0-r12,lr}                  // save register
        mrs      r12, cpsr                      // load status register to r0
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      r1, sp                         // get sp_usr/sys
        mov      r2, lr                         // get lr_usr/sys
        msr      cpsr, r12                      // back to SVC
        mrs      r3, spsr                       // get spsr_irq <> cpsr_irq
        stmfd    sp!, {r1-r3}                   // save those values to stack_irq
        mov      r0, sp                         // return pointer to stack_svc
        bl handle_swi                           // execute c-handler
        ldmfd sp!, {r1-r3}                      // everything rewind
        msr      spsr, r3                       // restore spsr
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      sp, r1                         // restore sp_usr/sys
        mov      lr, r2                         // restore lr_usr/sys
        msr      cpsr, r12                      // back to SVC status register
        ldmfd sp!, {r0-r12,lr}                  // write back register
        movs pc, lr                             // -> S-Bit will restore spsr to cpsr / end of handling

.global asm_handle_prefetch
asm_handle_prefetch:
        sub     lr, lr, #0x4                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mrs      r12, cpsr                      // load status register to r0
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      r1, sp                         // get sp_usr/sys
        mov      r2, lr                         // get lr_usr/sys
        msr      cpsr, r12                      // back to SVC
        mrs      r3, spsr                       // get spsr_irq <> cpsr_irq
        stmfd    sp!, {r1-r3}                   // save those values to stack_irq
        mov      r0, sp                         // return pointer to stack_svc
        bl handle_prefetch                      // execute c-handler
        ldmfd sp!, {r1-r3}                      // everything rewind
        msr      spsr, r3                       // restore spsr
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      sp, r1                         // restore sp_usr/sys
        mov      lr, r2                         // restore lr_usr/sys
        msr      cpsr, r12                      // back to SVC status register
        ldmfd sp!, {r0-r12,lr}                  // write back register
        movs pc, lr                             // -> S-Bit will restore spsr to cpsr / end of handling

.global asm_handle_data_abort
asm_handle_data_abort:
        sub      lr, lr, #0x8                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mrs      r12, cpsr                      // load status register to r0
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      r1, sp                         // get sp_usr/sys
        mov      r2, lr                         // get lr_usr/sys
        msr      cpsr, r12                      // back to IRQ
        mrs      r3, spsr                       // get spsr_irq <> cpsr_irq
        stmfd    sp!, {r1-r3}                   // save those values to stack_irq
        mov      r0, sp                         // return pointer to stack_irq
        bl handle_data_abort                    // execute c-handler
        ldmfd sp!, {r1-r3}                      // everything rewind
        msr      spsr, r3                       // restore spsr
        mrs      r12, cpsr                      // load status register
        orr      r0, r12, #0b1111               // adjust r0 to sys_mode (system mode = 11111)
        msr      cpsr, r0                       // change status register to sys_mode
        mov      sp, r1                         // restore sp_usr/sys
        mov      lr, r2                         // restore lr_usr/sys
        msr      cpsr, r12                      // back to IRQ status register
        ldmfd sp!, {r0-r12,lr}                  // write back register
        movs pc, lr                             // -> S-Bit will restore spsr to cpsr / end of handling

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
