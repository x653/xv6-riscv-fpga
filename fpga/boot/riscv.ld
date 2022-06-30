
MEMORY {
	/* the memory is 8k in size; */
	mem : ORIGIN = 0x00000000, LENGTH = 0x00003000
}

SECTIONS {
	.memory : {
		. = 0x000000;
		start*(.text);
		*(.text);
		*(*);
		end = .;
		. = ALIGN(4);
	} > mem
}

