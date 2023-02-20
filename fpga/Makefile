OBJS =  clint.v plic.v rv32.v soc.v spi.v sram.v uart.v vma.v

TOOLPREFIX = /opt/oss-cad-suite/bin/
LIBS = /opt/oss-cad-suite/lib/
YOSYS_ICE40 = /opt/oss-cad-suite/share/yosys/ice40/

YOSYS    = $(TOOLPREFIX)yosys
NEXTPNR  = $(TOOLPREFIX)nextpnr-ice40
ICEPACK  = $(TOOLPREFIX)icepack
ICEPROG  = $(TOOLPREFIX)iceprogduino
IVERILOG = $(TOOLPREFIX)iverilog
VVP      = $(TOOLPREFIX)vvp

PNRFLAGS = --hx8k --package ct256 --pcf ice40hx8k_evb.pcf -q

all:	build upload run

sim:	soc_tb.vcd

build:	hardware.bin

soc_tb.vcd:	$(OBJS)
	$(IVERILOG) -o soc_tb.out $(OBJS) soc_tb.v
	./soc_tb.out
	gtkwave soc_tb.vcd soc_tb.gtkw

hardware.bin:	$(OBJS) pll.v top.v ice40hx8k_evb.pcf
	$(YOSYS) -p "synth_ice40 -top top -json hardware.json" -q $(OBJS) pll.v top.v
	$(NEXTPNR) $(PNRFLAGS) --json hardware.json --asc hardware.asc
	$(ICEPACK) hardware.asc hardware.bin


upload:			hardware.bin
	$(ICEPROG) hardware.bin

run:	
	tio -m INLCRNL /dev/ttyACM0

clean:
	rm -f *.asc *.bin *.json *.vcd *.out

.PHONY: all clean



