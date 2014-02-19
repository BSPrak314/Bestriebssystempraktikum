ENTRY(_start)
SECTIONS
{
	
	. = 0x20100000;
	.usertext : 	{ bin/user/*(.text.startup)
		      	  bin/user/*(.text) }

	
	USERDATA = ( ( . /(0x10000) )+1 )*(0x10000);

	. = USERDATA;		     
	.rodata : 	{ bin/user/*(.rodata*) }
	.udata : 	{ bin/user/*(.data) }

	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }

	KERNELDATA = ( ( . /(64*1024)+1)*(64*1024) );
	. = KERNELDATA;
	.data : { *.(.data) }

}