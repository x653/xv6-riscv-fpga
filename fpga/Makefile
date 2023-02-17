TOOLPREFIX = ~/.apio/packages/tools-oss-cad-suite/bin/

hardware.json:	clint.v plic.v pll.v rv32.v soc.v spi.v sram.v top.v uart.v vma.v
	$(TOOLPREFIX)yosys -p "synth_ice40 -json hardware.json" -q clint.v plic.v pll.v rv32.v soc.v spi.v sram.v top.v uart.v vma.v

hardware.asc:	hardware.json
	$(TOOLPREFIX)nextpnr-ice40 --hx8k --package ct256 --json hardware.json --asc hardware.asc --pcf ice40hx8k_evb.pcf -q
hardware.bin:	hardware.asc
	$(TOOLPREFIX)icepack hardware.asc hardware.bin
upload:			hardware.bin
	$(TOOLPREFIX)iceprogduino hardware.bin
run:	
	tio -m INLCRNL /dev/ttyACM0
clean:
	rm hardware.*

