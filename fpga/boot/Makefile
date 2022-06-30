OBJS =  start.S main.c printf.c stdio.c sdcard.c 
CFLAG = -O0 -march=rv32i -mabi=ilp32 --freestanding -nostartfiles -Wl,-T,riscv.ld
CC = /home/micha/riscv64-unknown-elf-gcc-8.3.0-2020.04.0-x86_64-linux-ubuntu14/bin/riscv64-unknown-elf-gcc 
INCLUDE =
LIBS =

clean:
	rm -f *.elf *.o *.hex firmware.hex

firmware.elf:	$(OBJECTS)
	$(CC) $(CFLAG) $(OBJS) -o $@

%.hex:	%.elf
	readelf $< -x 1 | ./lsb2msb.py > $@

%.obj:  %.elf
	/home/micha/riscv64-unknown-elf-gcc-8.3.0-2020.04.0-x86_64-linux-ubuntu14/bin/riscv64-unknown-elf-objdump -d $?
	

firmware:	firmware.hex
	cp firmware.hex ../

