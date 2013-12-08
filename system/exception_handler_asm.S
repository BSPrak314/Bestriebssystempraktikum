  // Jump funcions to the interrupt handler functions at "exception_handler.c"
.section .text
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
        sub     lr, lr, #0x4                    // set lr to instruction where interrupt occurred
        stmfd sp!, {r0-r12,lr}                  // save register
        mov r0, sp                              // parameter to print out register information
        bl handle_swi                           // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        adds pc, r14, #0x4                      // jump back

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

.global asm_handle_irq
asm_handle_irq:
        stmfd sp!, {r0-r12,lr}                  // save register
        bl handle_irq                           // execute c-handler
        ldmfd sp!, {r0-r12,lr}                  // write back register
        subs pc, lr, #0x4

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
