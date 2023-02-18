OBJS =  start.S main.c printf.c stdio.c spi.c 

TOOLPREFIX = /opt/riscv/bin/riscv64-unknown-elf-

CFLAG = -Wall -O0 -march=rv32i -mabi=ilp32 --freestanding -nostartfiles -T riscv.ld

CC = $(TOOLPREFIX)gcc 

all:	firmware

firmware.elf:	$(OBJECTS)
	$(CC) $(CFLAG) $(OBJS) -o $@

%.hex:	%.elf
	readelf $< -x 1 | ./lsb2msb.py > $@

%.obj:  %.elf
	$(TOOLPREFIX)objdump -d $?
	
firmware:	firmware.hex
	cp firmware.hex ../

clean:
	rm -f *.elf *.o *.hex firmware.hex

.PHONY: all clean
