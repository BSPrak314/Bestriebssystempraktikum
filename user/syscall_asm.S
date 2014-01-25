
.global asm_syscall_write
asm_syscall_write:
	svc 0x0FF000
	mov pc, lr

.global asm_syscall_read
asm_syscall_read:
	svc 0x0F0000
	mov pc, lr

.global asm_syscall_kill
asm_syscall_kill:
        svc 0xFFFFFF
        mov pc, lr

.global asm_syscall_create
asm_syscall_create:
        svc 0x00000F
        mov pc, lr

.global asm_syscall_yield
asm_syscall_yield:
        svc 0x000FFF
        mov pc, lr

.global asm_syscall_wait
asm_syscall_wait:
	svc 0x0000FF
	mov pc, lr

.global asm_endless_loop
asm_endless_loop:
	push {r0-r12}
	b asm_endless_loop
