OBJS =  start.S main.c printf.c stdio.c spi.c

# Downloaded multilib from https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases
# added bin/ therein to PATH
TOOLPREFIX = riscv-none-elf-

CFLAG = -Wall -O0 -march=rv32i_zicsr -mabi=ilp32 --freestanding -nostartfiles -Triscv.ld -Wl,--no-warn-rwx-segments

CC = $(TOOLPREFIX)gcc

all:	firmware

firmware.elf:	$(OBJS) riscv.ld
	$(CC) $(CFLAG) $(OBJS) -o $@

firmware.hex:	firmware.elf
	$(TOOLPREFIX)objcopy -j .boot firmware.elf -O binary firmware.bin
	xxd -e firmware.bin | cut -c 11-46 > firmware.hex

%.obj:  %.elf
	$(TOOLPREFIX)objdump -d $?

firmware:	firmware.hex

	cp firmware.hex ../

clean:
	rm -f *.elf *.o *.hex firmware.hex

.PHONY: all clean
