OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80000000;
  .text :
  { 
	_entry = . ;
    *(.text*)
	*(.rodata*)
    . = ALIGN(0x1000);
    *(trampsec)
  }

  . = ALIGN(0x1000);
  PROVIDE(etext = .);

  /*
   * make sure end is after data and bss.
   */
  .data : {
    *(.data)
    *(.sdata)
  }
  .bss : {
    *(.bss)
    *(.sbss)
  }
  . = ALIGN(0x1000);
  PROVIDE(end = .);
}
