#include "stdio.h"
#include "printf.h"
#include "types.h"
#include "fs.h"
#include "elf.h"

unsigned char buf[BSIZE];
unsigned int indirect[BSIZE];
struct superblock sb;
struct dinode file;
struct dirent entry;
struct elfhdr elf;
struct proghdr prog;

void* memmove(void *dst, const void *src, uint n){
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}

void panic(char* m){
	printf("panic: %s\n",m);
	for(;;);
}

int getSuperblock(){
	printf("reading superblock\n");
	if (spi_rb(1,buf)) panic("read block\n");
	memmove(&sb,buf,sizeof(sb));
	printf("sb.magic: %p\n",sb.magic);	
	//printf("sb.size: %d\n",sb.size);	
	//printf("sb.inodestart: %p\n",sb.inodestart);
}

int getInode(int n){
	printf("reading inode %d\n",n);
	if (spi_rb(sb.inodestart,buf)) panic("read block\n");
	memmove(&file,buf+sizeof(file)*n,sizeof(file));
	//printf("file.type: %d\n",file.type);
	//printf("file.size: %d\n",file.size);
	//for (int i=0;i<12;i++){
	//	printf("file.addrs[%d]: %p\n",i,file.addrs[i]);
	//}
	if (spi_rb(file.addrs[12],(char*)indirect)) panic("read block\n");
	//for (int i=0;i<128;i++){
	//	printf("file.addrs[%d]: %p\n",i+12,indirect[i]);
	//}
}

void load(int off,int pa, int n){
	int block;
	while (n>0){
		if ((off>>9)<12) block = file.addrs[off>>9];
		else block = indirect[((off>>9)-12)]; 
		//printf("read block %d to %p n %d\n",block,pa,n);
		if (spi_rb(block,(char*)pa)) panic("read block\n");
		off+=BSIZE;
		pa+=BSIZE;
		n-=BSIZE;
	}	
}

int boot(){
	spi_init();
	getSuperblock(&sb);
	//getInode(1);
	//if (spi_rb(file.addrs[0],buf)) panic("read block\n");
	//for (int i=0;i<BSIZE/sizeof(entry);i++){
	//	memmove(&entry,buf+sizeof(entry)*i,sizeof(entry));
	//	printf("%d %s\n",entry.inum,entry.name);
	//}
	
	getInode(2);
	if (spi_rb(file.addrs[0],buf)) panic("read block\n");
	memmove(&elf,buf,sizeof(elf));
	printf("elf.magic: %p\n",elf.magic);
	//printf("elf.type: %d\n",elf.type);
	//printf("elf.machine: %d\n",elf.machine);
	//printf("elf.version: %d\n",elf.version);
	//printf("elf.entry: %p\n",elf.entry);
	//printf("elf.phnum: %d\n",elf.phnum);
	//printf("elf.phoff: %d\n",elf.phoff);
	//printf("elf.phentsize: %d\n",elf.phentsize);

	for (int i=0;i<elf.phnum;i++){
		//printf("prog header %d\n",i);
		memmove(&prog,buf+elf.phoff+i*sizeof(prog),sizeof(prog));
		//printf("prog.type: %d\n",prog.type);
		//printf("prog.off: %p\n",prog.off);
		//printf("prog.vaddr: %p\n",prog.vaddr);
		//printf("prog.paddr: %p\n",prog.paddr);
		//printf("prog.filesz: %p\n",prog.filesz);
		//printf("prog.memsz: %p\n",prog.memsz);
		//printf("prog.flags: %d\n",prog.flags);
		//printf("prog.align: %p\n",prog.align);
		load(prog.off,prog.paddr,prog.filesz);
	}	
	printf("jump to entry point %p\n\n",elf.entry);
	
}


void main(void)
{
	for (int i=0x80000000;i<0x80080000;i+=4){
		*(int*)i = 0;
	}
	uart_init();
	printf("\n ___ ___ ___  ___  __   __\n| _ \\_ _/ __|/ __|_\\ \\ / /\n|   /| |\\__ \\ (_|___\\ V / \n|_|_\\___|___/\\___|   \\_/  \n");
	printf("Processor: RV32ia @25MHz V1.0\n\n");
	printf("0x00000000 BOOT (8 KB)\n");
	printf("0x02000000 CLINT\n");
	printf("0x0C000000 PLIC\n");
	printf("0x10000000 UART\n");
	printf("0x20000000 SD-CARD\n");
	printf("0x80000000 RAM (512 KB)\n\n");

	boot();
	printf("Welcome to RV32ia 6th Edition UNIX\n");
	asm volatile("mv ra,%0" : : "r" (elf.entry));
	asm volatile("ret");
}
