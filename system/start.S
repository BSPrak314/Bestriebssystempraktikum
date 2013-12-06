.section .init

.global _start

_start:
		bl asm_init_stacks
		bl startOS
.Lend:
		b .Lend
