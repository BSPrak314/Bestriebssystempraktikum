
.section .text
.global asm_init_stacks
asm_init_stacks:
        
        // Setting stack pointers for all modes, full decending
        // all Stackpointer the use external RAM, starting at 0x24000000
        //
        //  - size of stackpointer for each mode 
        //     4 KByte = 4096 Byte = 0x1000 Byte
        // 
        // Processer mode when entering: 
        //   ->  supervisor
        // Processor mode when leaving:
        //   ->  system

        mov      r0, lr

        LDR      SP, =0x24000000        // set stackpointer for svc (supervisor mode = 10011)

        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits      
        ORR      R12, R12, #0x1B        // set mode to und (undefined mode = 11011)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x23FFF000        // set stackpointer        
        
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x17        // set mode to abt (abort mode = 10111)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x23FFE000        // set stackpointer
                    
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x12        // set mode to irq (interrupt request mode = 10010)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x23FFD000        // set stackpointer
              
        MRS      R12, CPSR              // load status register
        BIC      R12, R12, #0x1F        // clear last 5 bits
        ORR      R12, R12, #0x11        // set mode to fiq (fast interrupt mode = 10001)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x23FFC000        // set stackpointer
              
        MRS      R12, CPSR              // load status register
        ORR      R12, R12, #0x1F        // set mode to sys (system mode = 11111)
        MSR      CPSR, R12              // set status register 
        LDR      SP, =0x23FFB000        // set stackpointer

        MOV      PC, r0