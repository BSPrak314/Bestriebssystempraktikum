
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

.global asm_syscall_fork
asm_syscall_fork:
        svc 0x00000F
        mov pc, lr

.global asm_syscall_newProcess
asm_syscall_newProcess:
        svc 0xF0000F
        mov pc, lr

.global asm_syscall_yield
asm_syscall_yield:
        svc 0x000FFF
        mov pc, lr

.global asm_syscall_wait
asm_syscall_wait:
	svc 0x0000FF
	mov pc, lr

.global asm_syscall_get_memory
asm_syscall_get_memory:
	svc 0x111111
	mov pc, lr

.global asm_syscall_get_id
asm_syscall_get_id:
	svc 0x000001
	mov pc, lr

.global asm_syscall_get_localid
asm_syscall_get_localid:
	svc 0x000002
	mov pc, lr