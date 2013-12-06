
#include <dbgu.h>
#include <printf.h>
#include <exception_handler_asm.h>
#include <exception_handler.h>
#include <mem_ctrl.h>
#include <aic.h>
#include <sys_timer.h>
#include <led.h>

#define IVT_ADDR        0x00200000  //remapped (writable) area for ivt
#define JUMP_ADDR       0x00200020  
#define MC_RCR          0xFFFFFF00
#define LD_PC_PC_OFF18  0xE59FF018  //opcode pc = pc offset: 18

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
}

//IRQ Handler
int handle_irq(void)
{
        /* if Periodic Intervall Timer has triggered - sys_timer.c should deal with it */
        if( st_triggeredPIT() ){
                st_handlePIT();
        }
        /* if DBGU Interrupt has triggered - dbgu.c should deal with it  */
        if( dbgu_triggeredRXRDY() || dbgu_triggeredTXRDY() ){
                dbgu_dealWithInterrupts();
        }
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
/* very basic handler - printing out register information */
int handle_swi(struct reg_info *reg)
{        
        print("software interrupt at:\n");
        print_reginfo(reg);
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

        if( mc_isUndefAdress() ){
                print("Reseason: Undefined AdressSpace\n");
        }else if( mc_isMisalignment() ){
                print("Reseason: Data Misalignment\n");
        }else{
                print("Error while determining reason\n"); 
        }
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

//Prints registers when interrupt occurs
void print_cpsr( unsigned int cpsr )
{
        print(" cpsr : [<%b>]\n",cpsr);
}

//Prints registers when interrupt occurs
void print_register( unsigned int reg )
{       
        if(reg > 15)
                return;
        print("         cpsr : [<%x>]\n", reg);
}

//Prints registers when interrupt occurs
void print_reginfo( struct reg_info * reg)
{
        print("printing registers...\n");
        print("         lr : [<%x>]\n", reg->lr);
        print("r11: %x  r10: %x  r9 : %x\n",reg->r11, reg->r10, reg->r9);
        print("r8 : %x  r7 : %x  r6 : %x\n",reg->r8, reg->r7, reg->r6);
        print("r5 : %x  r4 : %x  r3 : %x\n",reg->r5, reg->r4, reg->r3);
        print("r2 : %x  r1 : %x  r0 : %x\n",reg->r2, reg->r1, reg->r0);
        print("> \n");
}