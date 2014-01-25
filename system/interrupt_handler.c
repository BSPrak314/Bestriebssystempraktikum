
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
        jump_add->RESET                 = (unsigned int)&asm_handle_reset;
        jump_add->undef_instr           = (unsigned int)&asm_handle_undef_inst;
        jump_add->sw_inter              = (unsigned int)&asm_handle_swi;
        jump_add->prefetch_abort        = (unsigned int)&asm_handle_prefetch;
        jump_add->data_abort            = (unsigned int)&asm_handle_data_abort;
        jump_add->IRQ                   = (unsigned int)&asm_handle_irq;
        jump_add->FIQ                   = (unsigned int)&asm_handle_fiq;

        /* currently no swi test -> threads.c should deal with swi interrups */
        swi_test = 0;
}

//IRQ Handler
int handle_irq( struct registerStruct * regStruct )
{
        // first check if some SystemTimer has triggered
        // SystemTimer should check the status register and deal with triggered Interrupts
        st_dealWithInterrupts( regStruct );
        // if DBGU Interrupt has triggered - dbgu.c should deal with it  
        // let the dbgu check and deal with them if necessary
        dbgu_dealWithInterrupts( regStruct );
        
        // clear Interrupt on Line1 and signal to aic interrupt handling is completed
        aic_clearInterrupt_nr(1);
        aic_endOfInterrupt(); 
        return 1;
}

//Reset-Handler
int handle_reset()
{
        print("reset interrupt\n");
        // provoke a reset by setting watchdog Value to 0
        // enabling Reset via Watchdog and Watchdog timer
        st_setWatchdogValue(0x00000001);
        st_enableWatchdogReset();
        st_enableWDT();
        return 1;
}

void interrupt_printInterruptInfo(char *interrupt_name, struct registerStruct *reg, unsigned int mode)
{
        print("\n  %s  ",interrupt_name);

        switch( mode ){
        case 0x1F :
                print(" in SYSTEM Mode\n");
                break;
        case 0x10 :
                print(" in  USER  Mode\n");
                break;
        case 0x11 :
                print(" in FAST INTERRUPT  Mode\n");
                break;
        case 0x12 :
                print(" in INTERRUPT  Mode\n");                
                break;
        case 0x13 :
                print(" in SUPERVISOR Mode\n");
                break;
        case 0x17 :
                print(" in ABORT      Mode\n");
                break;
        case 0x1B :
                print(" in UNDEFINED  Mode\n");
                break;
        }

        print("  Register Values :      \n");
        print_RegisterStruct(reg);
        print("\n");
}

void interrupt_printHandling(unsigned int mode)
{
        if( mode == 0x10){
                unsigned int runningID = thread_getRunningID();
                unsigned int runningPos = (runningID << 24);
                runningPos = (runningPos >> 24);
                print("\n  Handling:\n");
                print("  Kill causing Thread\n");
                print("  Position: %x\n",runningPos);
                print("  ID      : %x\n",runningID);
                print("\n");
                return;
        }
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        print("* JUMP BACK *\n");
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
}

//Undefined Instruction-Handler
/* very basic handler - printing out register information */
int handle_undef_inst(struct registerStruct *reg)
{       
        unsigned int mode = reg->cpsr & 0x0000001F;
        interrupt_printInterruptInfo("UNDEFINED INSTRUCTION", reg, mode);
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
        unsigned int instr = *(unsigned int *)(reg->pc-4);
        instr = (instr << 8);
        instr = (instr >> 8);

        // swi_test is above and shared with lib_system/systemtest.c
        if( ! swi_test ){
                thread_dealWithSWI(instr, reg);
        }else{
                print("software interrupt has infoTag : %x\n",instr);
                print("software interrupt at:\n");
                print_RegisterStruct(reg);
        }

        return 1;
}
//Prefetch Handler
/* very basic handler - printing out register information */
int handle_prefetch(struct registerStruct *reg)
{        
        unsigned int mode = reg->cpsr & 0x0000001F;
        interrupt_printInterruptInfo("PREFETCH  ABORT      ", reg, mode);

        interrupt_printHandling(mode);
        if( mode == 0x10 ){
                return thread_kill(reg);
        }
        return 1;
}
//Data Abort Handler
int handle_data_abort(struct registerStruct *reg)
{
        unsigned int mode = reg->cpsr & 0x0000001F;
        interrupt_printInterruptInfo("DATA      ABORT      ", reg, mode);
        int type = mc_getAbortType();
        if( type <= 0 && type < 9 && type != 3 && type != 5 && type != 7 )
                print("  Modified Virtual Address was:\nMVA:  [> %x <]\n",mc_getAbortAdress());
        print("  Data Abort caused by:\n");
        switch( type ){
        case 1 :
                print(" -> Alignment abort\n");
                break;
        case 2 : 
                print(" -> Translation abort for section\n");
                break;
        case 3 :
                print(" -> Translation abort for page\n");
                break;
        case 4 : 
                print(" -> Domain abort for section\n");
                break;
        case 5 :
                print(" -> Domain abort for page\n");
                break;
        case 6 : 
                print(" -> Permission abort for section\n");
                break;
        case 7 : 
                print(" -> Permission abort for page\n");
                break;
        case 8 : 
                print(" -> External abort for section,\n   noncachable nonbufferable access or noncachable bufferable read\n");
                break;
        case 9 : 
                print(" -> External abort for page,\n   noncachable nonbufferable access or noncachable bufferable read\n");
                break;
        default : 
                print("!!! ERROR in mc_getAbortType !!!!!\n");
        }
        switch( mc_getAbortStatus() ){
        case 0 :
                print(" -> during data read\n");
                break;
        case 1 :
                print(" -> during data write\n"); 
                break;
        case 2 :
                print(" -> during code fetch\n"); 
                break;
        default : ;
        }

        interrupt_printHandling(mode);
        if( mode == 0x10 ){
                return thread_kill(reg);
        }
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