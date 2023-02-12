OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY {
	/* the memory is 12k in size; */
	mem : ORIGIN = 0x00001000, LENGTH = 0x00003000
}

SECTIONS {
	.mem : {
		. = 0x000000;
		start*(.text);
		*(.text);
		*(*);
		end = .;
		. = ALIGN(4);
	} > mem
}

