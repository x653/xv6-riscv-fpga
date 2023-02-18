#include "stdio.h"
#include "printf.h"
#include "types.h"
#include "fs.h"
#include "elf32.h"

extern void* end;

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

void getSuperblock(){
	spi_init();
	if (spi_rb(1,buf)) panic("getSuperblock()\n");
	memmove(&sb,buf,sizeof(sb));
}

void getELFHeader(){
	if (spi_rb(file.addrs[0],buf)) panic("getELFHeader()\n");
	memmove(&elf,buf,sizeof(elf));
}

void getInode(int n){
	if (spi_rb(sb.inodestart,buf)) panic("read block\n");
	memmove(&file,buf+sizeof(file)*n,sizeof(file));
	if (spi_rb(file.addrs[12],(unsigned char*)indirect)) panic("getInode()\n");
}

void load(int off,int pa, int n){
	int block;
	while (n>0){
		if ((off>>9)<12) block = file.addrs[off>>9];
		else block = indirect[((off>>9)-12)]; 
		if (spi_rb(block,(unsigned char*)pa)) panic("load()\n");
		off+=BSIZE;
		pa+=BSIZE;
		n-=BSIZE;
	}	
}

void main(void)
{
	uart_init();
	printf("\n ___ ___ ___  ___  __   __\n| _ \\_ _/ __|/ __|_\\ \\ / /\n|   /| |\\__ \\ (_|___\\ V / \n|_|_\\___|___/\\___|   \\_/  \n\n");
	printf("Processor: rv32ia @32MHz V1.2\n\n");
	printf("0x00001000 BOOT (12 KB)\n");
	printf("0x02000000 CLINT\n");
	printf("0x0C000000 PLIC\n");
	printf("0x10000000 UART\n");
	printf("0x10001000 SD-CARD\n");
	printf("0x80000000 RAM (512 KB)\n\n");
	
	for (int i=0x80000000;i<0x80080000;i+=4){
		*(int*)i = 0;
	}
	
	printf("sb_magic 0x10203040");
	while (sb.magic!=0x10203040){
		getSuperblock();	
		printf(".");
	}
	printf("OK\ne_magic  0x464c457f");
	int inode=0;
	while((elf.magic!=0x464c457f) || (elf.entry!=0x80000000)){
		getInode(++inode);
		getELFHeader();
		printf(".");
	}
	printf("OK\n   p_type     p_addr     p_filesz   load\n");
	for (int i=0;i<elf.phnum;i++){
		memmove(&prog,buf+elf.phoff+i*sizeof(prog),sizeof(prog));
		printf("%d  %p %p %p ",i,prog.type,prog.paddr,prog.filesz);
		if (prog.type==1) {
			load(prog.off,prog.paddr,prog.filesz);
			printf("OK");
		}
		printf("\n");
	}	
	printf("e_entry  %p\n\n",elf.entry);

	printf("Welcome to rv32ia 6th Edition UNIX\n");
	asm volatile("mv ra,%0" : : "r" (elf.entry));
	asm volatile("ret");
}
