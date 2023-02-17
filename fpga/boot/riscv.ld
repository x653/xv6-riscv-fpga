OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY {
	/* the memory is 12k in size; */
	mem (rwx): ORIGIN = 0x00001000, LENGTH = 0x00003000
}

SECTIONS {
	.memory : {
		_entry = .;
		*(.text);
		. = ALIGN(4);
		*(.rodata);
		. = ALIGN(4);
		*(.data);
		. = ALIGN(4);
		*(.bss);
		. = ALIGN(4);
		*(.sbss);
		. = ALIGN(4);
	} > mem
}

