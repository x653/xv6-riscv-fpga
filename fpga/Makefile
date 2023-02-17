OBJS =  clint.v plic.v pll.v rv32.v soc.v spi.v sram.v top.v uart.v vma.v

TOOLPREFIX = /opt/oss-cad-suite/bin/

YOSYS   = $(TOOLPREFIX)yosys
NEXTPNR = $(TOOLPREFIX)nextpnr-ice40
ICEPACK = $(TOOLPREFIX)icepack
ICEPROG = $(TOOLPREFIX)iceprogduino

PNRFLAGS = --hx8k --package ct256 --pcf ice40hx8k_evb.pcf -q


all:	upload

hardware.json:	$(OBJS)
	$(YOSYS) -p "synth_ice40 -top top -json hardware.json" -q $(OBJS)

hardware.asc:	hardware.json
	$(NEXTPNR) $(PNRFLAGS) --json hardware.json --asc hardware.asc

hardware.bin:	hardware.asc
	$(ICEPACK) hardware.asc hardware.bin

upload:			hardware.bin
	$(ICEPROG) hardware.bin

run:	
	tio -m INLCRNL /dev/ttyACM0

clean:
	rm -f hardware.*

.PHONY: all clean



