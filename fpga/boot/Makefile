OBJS =  start.S main.c printf.c stdio.c spi.c 

TOOLPREFIX = /opt/riscv/bin/riscv64-unknown-elf-

CFLAG = -Wall -O0 -march=rv32i -mabi=ilp32 --freestanding -nostartfiles -T riscv.ld

CC = $(TOOLPREFIX)gcc 

all:	firmware

firmware.elf:	$(OBJS)
	$(CC) $(CFLAG) $(OBJS) -o $@

firmware.hex:	firmware.elf
	$(TOOLPREFIX)objcopy -j .mem firmware.elf -O binary firmware.bin
	xxd -e firmware.bin | cut -c 11-46 > firmware.hex

%.obj:  %.elf
	$(TOOLPREFIX)objdump -d $?
	
firmware:	firmware.hex

	cp firmware.hex ../

clean:
	rm -f *.elf *.o *.hex firmware.hex

.PHONY: all clean
