OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY {
	/* the memory is 12k in size; */
	mem (rwx): ORIGIN = 0x00001000, LENGTH = 0x00003000
}

SECTIONS {
	.text : {
		_entry = .;
		*(.text);
		*(.rodata);
		*(.data);
		*(.bss);
		*(.sbss);
		. = ALIGN(4);
	} > mem
}

