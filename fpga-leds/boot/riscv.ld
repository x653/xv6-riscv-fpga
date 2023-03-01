OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY {
	/* the memory is 12k in size; */
	boot (rwx): ORIGIN = 0x00001000, LENGTH = 0x00003000
}

SECTIONS {
	.mem : {
		_entry = .;
		*(.text);
		. = ALIGN(16);
		*(.srodata);
		. = ALIGN(16);
		*(.rodata);
		. = ALIGN(16);
		*(.sdata);
		. = ALIGN(16);
		*(.data);
		. = ALIGN(16);
		*(.sbss);
		. = ALIGN(16);
		*(.bss);
		. = ALIGN(16);
	} > boot
}

