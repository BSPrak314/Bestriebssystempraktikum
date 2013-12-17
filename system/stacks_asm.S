
.section .text
.global asm_init_stacks
asm_init_stacks:
        
        //Setting stack pointers for all modes, decending
        //Using the internal RAM, adjust comments the use external RAM
        //
        // Processer mode when entering: 
        // supervisor mode
        // Processor mode when leaving:
        // system mode

        mov      r0, lr

        LDR      SP, =0x00300000        // set stackpointer for svc (supervisor mode = 10011)
        //Using the external RAM
        //LDR      SP, =0x24000000      // set stackpointer

        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits      
        ORR      R12, R12, #0x1B        // set mode to und (undefined mode = 11011)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FF000        // set stackpointer
        //Using the external RAM
        //LDR      SP, =0x23FFF000      // set stackpointer        
        
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x17        // set mode to abt (abort mode = 10111)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FE000
        //Using the external RAM
        //LDR      SP, =0x23FFE000      // set stackpointer
                    
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x12        // set mode to irq (interrupt request mode = 10010)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FD000        // set stackpointer
        //Using the external RAM
        //LDR      SP, =0x23FFD000      // set stackpointer
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x11        // set mode to fiq (fast interrupt mode = 10001)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FC000        // set stackpointer
        //Using the external RAM
        //LDR      SP, =0x23FFC000      // set stackpointer
              
        MRS      R12, CPSR              // load status register
        ORR      R12, R12, #0x1F        // set mode to sys (system mode = 11111)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x002FB000        // set stackpointer
        //Using the external RAM
        //LDR      SP, =0x23FFB000      // set stackpointer

        MOV      PC, r0