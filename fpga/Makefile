OBJS =  clint.v plic.v rv32.v soc.v spi.v sram.v uart.v vma.v

TOOLPREFIX = /opt/oss-cad-suite/bin/

PNRFLAGS = --hx8k --package ct256 --pcf ice40hx8k_evb.pcf -q

all:	build upload run

sim:	soc_tb.vcd

build:	hardware.bin

soc_tb.vcd:	$(OBJS) firmware.hex
	$(TOOLPREFIX)iverilog -o soc_tb.out $(OBJS) soc_tb.v
	./soc_tb.out
	$(TOOLPREFIX)gtkwave soc_tb.vcd soc_tb.gtkw

hardware.bin:	$(OBJS) pll.v top.v ice40hx8k_evb.pcf firmware.hex
	$(TOOLPREFIX)yosys -p "synth_ice40 -top top -json hardware.json" -q $(OBJS) pll.v top.v
	$(TOOLPREFIX)nextpnr-ice40 $(PNRFLAGS) --json hardware.json --asc hardware.asc
	$(TOOLPREFIX)icepack hardware.asc hardware.bin


upload:	hardware.bin
	$(TOOLPREFIX)iceprogduino -t 
	$(TOOLPREFIX)iceprogduino hardware.bin

run:	
	tio -m INLCRNL /dev/ttyACM0

clean:
	rm -f *.asc *.bin *.json *.vcd *.out

.PHONY: all clean



