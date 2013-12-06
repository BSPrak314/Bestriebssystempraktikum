
.section .text
.global asm_init_stacks
asm_init_stacks:
        //Setting stack pointers for all modes, decending
        
        mov      r0, lr

        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
      
        ORR      R12, R12, #0x1B        // set mode to und (undefined)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x00300000        // set stackpointer
        //Using the external RAM
        //LDR      SP, =0x24000000        // set stackpointer
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x17        // set mode to abt (abort)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FF000
        //Using the external RAM
        //LDR      SP, =0x23FFF000
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x13        // set mode to svc (supervisor)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FE000
        //Using the external RAM
        //LDR      SP, =0x23FFE000
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x12        // set mode to irq (interrupt request)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FD000
        //Using the external RAM
        //LDR      SP, =0x23FFD000
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x11        // set mode to fiq (fast interrupt)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FC000
        //Using the external RAM
        //LDR      SP, =0x23FFC000
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x1F        // set mode to sys (system mode)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FB000
        //Using the external RAM
        //LDR      SP, =0x23FFB000

        MOV      PC, r0