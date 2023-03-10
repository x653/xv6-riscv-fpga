K=kernel
U=user

OBJS = \
  $K/entry.o \
  $K/start.o \
  $K/console.o \
  $K/printf.o \
  $K/uart.o \
  $K/kalloc.o \
  $K/spinlock.o \
  $K/string.o \
  $K/main.o \
  $K/vm.o \
  $K/proc.o \
  $K/swtch.o \
  $K/trampoline.o \
  $K/trap.o \
  $K/syscall.o \
  $K/sysproc.o \
  $K/bio.o \
  $K/fs.o \
  $K/log.o \
  $K/sleeplock.o \
  $K/file.o \
  $K/pipe.o \
  $K/exec.o \
  $K/sysfile.o \
  $K/kernelvec.o \
  $K/plic.o \
  $K/sdcard.o \

# riscv64-unknown-elf- or riscv64-linux-gnu-
# perhaps in /opt/riscv/bin
TOOLPREFIX = /opt/riscv/bin/riscv64-unknown-elf-
LIBGCC = /opt/riscv/lib/gcc/riscv64-unknown-elf/12.1.0/rv32i/ilp32

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer #-ggdb -gdwarf-2
CFLAGS += -march=rv32ia_zicsr
CFLAGS += -mabi=ilp32
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -nostdlib -mno-relax #-fno-common #-s
CFLAGS += -I.

LDFLAGS = -z max-page-size=4096 -melf32lriscv #-s

$K/kernel: $(OBJS) $K/kernel.ld $U/initcode
	$(LD) $(LDFLAGS) -T$K/kernel.ld -o $K/kernel $(OBJS) -L$(LIBGCC) -lgcc
	$(OBJDUMP) -S $K/kernel > $K/kernel.asm
	$(OBJDUMP) -t $K/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $K/kernel.sym

$U/initcode: $U/initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -Ikernel -c $U/initcode.S -o $U/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $U/initcode.out $U/initcode.o -L$(LIBGCC) #-lgcc
	$(OBJCOPY) -S -O binary $U/initcode.out $U/initcode
	$(OBJDUMP) -S $U/initcode.o > $U/initcode.asm

tags: $(OBJS) _init
	etags *.S *.c

$K/entry.o:	$K/entry32.S
	$(CC) $(CFLAGS) -c -o $K/entry.o $K/entry32.S
$K/swtch.o:	$K/swtch32.S
	$(CC) $(CFLAGS) -c -o $K/swtch.o $K/swtch32.S
$K/trampoline.o:	$K/trampoline32.S
	$(CC) $(CFLAGS) -c -o $K/trampoline.o $K/trampoline32.S
$K/kernelvec.o:	$K/kernelvec32.S
	$(CC) $(CFLAGS) -c -o $K/kernelvec.o $K/kernelvec32.S
	
ULIB = $U/ulib.o $U/usys.o $U/printf.o $U/umalloc.o

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^ -L$(LIBGCC) -lgcc -s
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

$U/usys.S : $U/usys.pl
	perl $U/usys.pl > $U/usys.S

$U/usys.o : $U/usys.S
	$(CC) $(CFLAGS) -c -o $U/usys.o $U/usys.S

$U/_forktest: $U/forktest.o $(ULIB)
	# forktest has less library code linked in - needs to be small
	# in order to be able to max out the proc table.
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $U/_forktest $U/forktest.o $U/ulib.o $U/usys.o  -L$(LIBGCC) #-lgcc
	$(OBJDUMP) -S $U/_forktest > $U/forktest.asm

mkfs/mkfs: mkfs/mkfs.c $K/fs.h
	gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

UPROGS=\
	$U/_cat\
	$U/_echo\
	$U/_forktest\
	$U/_grep\
	$U/_init\
	$U/_kill\
	$U/_ln\
	$U/_ls\
	$U/_mkdir\
	$U/_rm\
	$U/_sh\
	$U/_wc\
	$U/_zombie\
	$U/_stressfs\
	$U/_uptime\
	$U/_usertests\
	$U/_hello\
	$U/_lisp\
	$U/_mem\
	$U/_ping\

FILES=\
	$U/_let\
	$U/_sqrt\
	$U/_fib\

fs.img: mkfs/mkfs README $(UPROGS)
	cp kernel/kernel xv6.elf
	mkfs/mkfs fs.img xv6.elf README $(FILES) $(UPROGS)

-include kernel/*.d user/*.d

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.o */*.d */*.asm */*.sym \
	$U/initcode $U/initcode.out $K/kernel xv6.elf fs.img \
	mkfs/mkfs .gdbinit \
        $U/usys.S \
	$(UPROGS)

ifndef CPUS
CPUS := 1
endif
