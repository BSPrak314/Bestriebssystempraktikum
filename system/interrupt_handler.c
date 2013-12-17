
#include <dbgu.h>
#include <printf.h>
#include <thread.h>
#include <interrupt_handler_asm.h>
#include <reg_operations_asm.h>
#include <interrupt_handler.h>
#include <mem_ctrl.h>
#include <aic.h>
#include <sys_timer.h>

#define IVT_ADDR        0x00200000  //remapped (writable) area for ivt
#define JUMP_ADDR       0x00200020  
#define LD_PC_PC_OFF18  0xE59FF018  //opcode pc = pc offset: 18

unsigned int swi_test;

struct ivt{
        unsigned int RESET;
        unsigned int undef_instr;
        unsigned int sw_inter;
        unsigned int prefetch_abort;
        unsigned int data_abort;
        unsigned int empty;
        unsigned int IRQ;
        unsigned int FIQ;
};

static volatile
struct ivt * const jump_inst = (struct ivt *)IVT_ADDR;

static volatile
struct ivt * const jump_add = (struct ivt *)JUMP_ADDR;  

void init_IVT(void)
{
        //Putting struct on ivt
        jump_inst->RESET                = LD_PC_PC_OFF18;
        jump_inst->undef_instr          = LD_PC_PC_OFF18;
        jump_inst->sw_inter             = LD_PC_PC_OFF18;
        jump_inst->prefetch_abort       = LD_PC_PC_OFF18;
        jump_inst->data_abort           = LD_PC_PC_OFF18;
        jump_inst->IRQ                  = LD_PC_PC_OFF18;
        jump_inst->FIQ                  = LD_PC_PC_OFF18;
        //Putting struct to ivt + 0x18 for junp to (asm-)handling
        jump_add->RESET                 = (unsigned int)asm_handle_reset;
        jump_add->undef_instr           = (unsigned int)asm_handle_undef_inst;
        jump_add->sw_inter              = (unsigned int)asm_handle_swi;
        jump_add->prefetch_abort        = (unsigned int)asm_handle_prefetch;
        jump_add->data_abort            = (unsigned int)asm_handle_data_abort;
        jump_add->IRQ                   = (unsigned int)asm_handle_irq;
        jump_add->FIQ                   = (unsigned int)asm_handle_fiq;

        /* currently no swi test -> threads.c should deal with swi interrups */
        swi_test = 0;
}

//IRQ Handler
int handle_irq( struct registerStruct * regStruct )
{
        //print("regStruct from IRQ\n");
        //printRegisterStruck(regStruct);
    
        /* first check if some SystemTimer has triggered
         * SystemTimer should check the status register and deal with triggered Interrupts */
        st_dealWithInterrupts( regStruct );
        
        /* if DBGU Interrupt has triggered - dbgu.c should deal with it  
         * let the dbgu check and deal with them if necessary  */
        dbgu_dealWithInterrupts( regStruct );
        
        /* clear Interrupt on Line1 and signal to aic interrupt handling is completed */
        aic_clearInterrupt_nr(1);
        aic_endOfInterrupt(); 
        return 1;
}

//Reset-Handler
int handle_reset()
{
        print("reset interrupt\n");
        /* provoke a reset by setting watchdog Value to 0
         * enabling Reset via Watchdog and Watchdog timer */
        st_setWatchdogValue(0x00000001);
        //st_enableWatchdogReset();
        st_enableWDT();
        return 1;
}
//Undefined Instruction-Handler
/* very basic handler - printing out register information */
int handle_undef_inst(struct reg_info *reg)
{       
        print("undefined instruction at:\n");
        print_reginfo(reg);
        return 1;
}
//Software Interrupt Handler
/* if swi_test = 1 => printing out register information 
 * if swi_test = 0 => thread.c should handle called SWI instruction */
int handle_swi(struct registerStruct *reg)
{        
        // extract coded SWI instruction from link register
        // performing shifts to clear out swi operation, 
        // leaving only offset coding the instruction 
        unsigned int * address = (unsigned int * )(reg->pc - 4);
        unsigned int instr = *address;
        
        instr = (instr << 8);
        instr = (instr >> 8);

        // swi_test is defined here and shared with lib/systemtest.c
        if( swi_test ){
                print("software interrupt has infoTag : %x\n",instr);
                print("software interrupt at:\n");
                printRegisterStruck(reg);
        }else{
                thread_dealWithSWI(instr, reg);
        }
        return 1;
}
//Prefetch Handler
/* very basic handler - printing out register information */
int handle_prefetch(struct reg_info *reg)
{        
        print("prefetch abort at:\n");
        print_reginfo(reg);
        return 1;
}
//Data Abort Handler
/* basic handler - printing out register information 
 * and some info about the sorce of the data abort */
int handle_data_abort(struct reg_info *reg)
{
        print("data abort during:\n");

        
        switch( mc_getAbortType() ){
        case 0 : {
                print("data read\n"); break;
                }
        case 1 : {
                print("data write\n"); break;
                }
        case 2 : {
                print("code fetch\n"); break;
                }
        case 3 : {
                print(">reserved<\n"); break;
                }
        default : {
                print("Error in mc_getAbortType\n");
                }
        }

        print("at Adress %x \n",mc_getAbortAdress());

        /*
        if( mc_isUndefAdress() ){
                print("Reseason: Undefined AdressSpace\n");
        }else if( mc_isMisalignment() ){
                print("Reseason: Data Misalignment\n");
        }else{
                print("Error while determining reason\n"); 
        }
        */
        print_reginfo(reg);
        
        return 1;
}

//FIQ Handler
/* very basic handler - printing out register information */
int handle_fiq(struct reg_info *reg)
{        
        print("fast interrupt\n");
        print_reginfo(reg);
        return 1;
}

//Spurious Handler
/* very very basic handler - printing out interrupt occurred information */
int handle_spurious( void )
{        
        print("spurious interrupt\n");
        return 1;
}

//Prints CPSR in binaer coding, buffered printing, so output will display when interrupt handling is done
void print_cpsr( void )
{
        unsigned int cpsr = asm_getCPSR();
        print("cpsr : [<%x>]\n> \n",cpsr);
}

//Prints register nr reg, buffered printing, so output will display when interrupt handling is done
void print_register( unsigned int reg )
{       
        if( reg > 12 && reg != 14 ){
                print("print register can only print register r0 to r12 and r14 := lr \nPlease retry with a register number between 0 and 12 or 14\n >\n");
                return;
        }
        struct reg_info *registers = (struct reg_info *)asm_getRegisters();
        
        unsigned int reg_contains = registers->lr;
        if(reg < 13)
                reg_contains = registers->r[reg];
        
        print("reg_%x : [<%x>]\n", reg,reg_contains);
}

//Prints all registers, buffered printing, so output will display when interrupt handling is done
void print_reginfo( struct reg_info * reg)
{
        print("printing registers...\n");
        print("         lr : [<%x>]\n", reg->lr);
        print("r11: %x  r10: %x  r9 : %x\n",reg->r[11], reg->r[10], reg->r[9]);
        print("r8 : %x  r7 : %x  r6 : %x\n",reg->r[8], reg->r[7], reg->r[6]);
        print("r5 : %x  r4 : %x  r3 : %x\n",reg->r[5], reg->r[4], reg->r[3]);
        print("r2 : %x  r1 : %x  r0 : %x\n",reg->r[2], reg->r[1], reg->r[0]);
        print("> \n");
}

//Prints all registers, printing via polling, so output will display during interrupt handling
void printRegisterStruck( struct registerStruct * reg)
{
        print("printing registers...\n");
        print("         lr_irq   : [<%x>]\n", reg->pc);
        print("         sp_sys   : [<%x>]\n", reg->sp);
        print("         lr_sys   : [<%x>]\n", reg->lr);
        print("         cpsr_sys : [<%x>]\n", reg->cpsr);
        print("r11: %x  r10: %x  r9 : %x\n",reg->r[11], reg->r[10], reg->r[9]);
        print("r8 : %x  r7 : %x  r6 : %x\n",reg->r[8], reg->r[7], reg->r[6]);
        print("r5 : %x  r4 : %x  r3 : %x\n",reg->r[5], reg->r[4], reg->r[3]);
        print("r2 : %x  r1 : %x  r0 : %x\n",reg->r[2], reg->r[1], reg->r[0]);
        print("> \n");
}