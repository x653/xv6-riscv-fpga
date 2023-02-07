# xv6-riscv-fpga

Build a RISC-V computer system on fpga iCE40HX8K-EVB and run UNIX xv6 using only FOSS (free and open source hard- and software).

# install riscv toolchain
https://github.com/stnolting/riscv-gcc-prebuilt
rv64imc multilib: rv32i

## Build hardware

```
$ cd fpga
$ cd boot
$ make
$ cd ..
$ apio upload
```

## Build software

```
$ cd xv6-riscv
$ make
$ make fs.img
```

## Run UNIX xv6 on RISC-V

```
$ tio -m INLCRNL /dev/ttyACM0

 ___ ___ ___  ___  __   __
| _ \_ _/ __|/ __|_\ \ / /
|   /| |\__ \ (_|___\ V / 
|_|_\___|___/\___|   \_/  

Processor: rv32ia @32MHz V1.0

0x00000000 BOOT (12 KB)
0x02000000 CLINT
0x0C000000 PLIC
0x10000000 UART
0x20000000 SD-CARD
0x80000000 RAM (512 KB)

reading superblock
sb.magic: 0x10203040
reading inode 2
reading ELF header
elf.magic: 0x464c457f
jump to entry point 0x80000000

Welcome to rv32ia 6th Edition UNIX

xv6 kernel is booting

init: starting sh
$ 
```

## Run ls and cat

```
$ ls
.              1 1 1296
..             1 1 1296
xv6.kernel     2 2 45676
README         2 3 1982
let            2 4 102
sqrt           2 5 443
sqrt2          2 6 467
fib            2 7 429
cat            2 8 4016
echo           2 9 3708
forktest       2 10 2128
grind          2 11 7952
grep           2 12 4612
init           2 13 3988
kill           2 14 3672
ln             2 15 3704
ls             2 16 4536
mkdir          2 17 3740
rm             2 18 3736
sh             2 19 7936
wc             2 20 4176
zombie         2 21 3592
stressfs       2 22 3896
uptime         2 23 3820
usertests      2 24 40660
lisp           2 25 23432
hello          2 26 3640
mem            2 27 4184
console        3 28 0
$ cat README
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)). See also https://pdos.csail.mit.edu/6.828/, which
provides pointers to on-line resources for v6.

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by
Silas Boyd-Wickizer, Anton Burtsev, Dan Cross, Cody Cutler, Mike CAT,
Tej Chajed, eyalz800, Nelson Elhage, Saar Ettinger, Alice Ferrazzi,
Nathaniel Filardo, Peter Froehlich, Yakir Goaron,Shivam Handa, Bryan
Henry, Jim Huang, Alexander Kapshuk, Anders Kaseorg, kehao95, Wolfgang
Keller, Eddie Kohler, Austin Liew, Imbar Marinescu, Yandong Mao, Matan
Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark Morrissey, mtasm, Joel
Nider, Greg Price, Ayan Shafqat, Eldar Sehayek, Yongming Shen, Cam
Tenny, tyfkda, Rafael Ubal, Warren Toomey, Stephen Tu, Pablo Ventura,
Xi Wang, Keiichi Watanabe, Nicolas Wolovick, wxdao, Grant Wu, Jindong
Zhang, Icenowy Zheng, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2019 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching
operating system for MIT's 6.828, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu. Once they are installed, and in your shell
search path, you can run "make qemu".
$ 
```

## Run lisp on xv6 on RISC-V

```
$ lisp
lisp v6
sizeof pair: 2
size of number: 4
size of float: 4
size of (char*): 4
lisp> (+ 2 4)
6
lisp> 
```
