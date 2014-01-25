ENTRY(_start)
SECTIONS
{
	. = 0x20100000;
	.newtext : { bin/user/*.o(.text) }
	.newdata : { bin/user/*.o(.rodata*) }

	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }
}