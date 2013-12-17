
.global asm_swi_call_kill
asm_swi_call_kill:
        svc 0xFFFFFF

.global asm_swi_call_create
asm_swi_call_create:
        svc 0x00000F

.global asm_swi_call_yield
asm_swi_call_yield:
        svc 0x000FFF