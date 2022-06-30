#include "stdio.h"
#include "printf.h"

char toHex(char c){
	if (c>='0' & c<='9') return c-'0';
	if (c>='a' & c<='f') return c-'a'+10;
	return -1;
}

void dump(int p,int len){
	printf("Hex dump of section %p (%d bytes)\n",p,len);
	for (int i=0;i<len;i+=16){
		printf("  %p ",p+i);
		for (int ii=0;ii<16;ii++){
			printf("%b",*(unsigned char *)(p+i+ii));
			if (!(~ii & 0x3)) printf(" ");
		}
		for (int ii=0;ii<16;ii++){
			unsigned char b = *(unsigned char *)(p+i+ii);
			if ((b>=32) & (b<127)) putchar(b);
			else putchar('.');
		}
		printf("\n");
	}
}

void main(void)
{
	for (int i=0x80000000;i<0x80080000;i+=4){
		*(int*)i = 0;
	}
	uart_init();
	printf("\nRISC-V\n");
	int mode = 0;
	unsigned int p=0;
	int x=0;
	int cc=0;
	int d=4;
	int echo=0;
	while (mode >= 0){
		int c = getchar();
		if (echo) putchar(c);
		else if (c=='\n') putchar('.');
		if (mode==0){
			if (c==' ');
			else if (c=='0') {mode=2;d=4;}
			else if (c=='E') {echo=!echo;mode=100;}
			else if (c=='D') {d=42;mode=1;}
			else if (c=='G') {d=-1;mode=1;}
			else if (c!='\n') mode=100;
		}else if (mode==1){
			if (c=='0') mode=2;
			else if (c=='\n') mode=0;
		}else if (mode==2){
			if (c=='x') {mode=3;p=0;}
			else if (c=='\n') mode=0;
			else mode=100;
		}else if (mode==3){
			if ((c==' ') || (c=='\n')) {mode=d;cc=0;}
			else {p<<=4;p|=toHex(c);}
		}else if (mode==4){
			if (c=='\n') mode=0;
			else if (c==' ') mode=4;
			else if (cc==16) mode=100;
			else {mode=5;x=toHex(c)<<4;}
		}else if (mode==5){
			if (c=='\n') mode = 0;
			else {mode=4;x|=toHex(c);*(unsigned char*)(p+cc)=x;++cc;}
		}else if (mode==42){
			if (c=='\n') {dump(p,cc);mode=0;}
			else {cc<<=4;cc|=toHex(c);}
		}else if (mode==43){
			mode=-1;
		}else if(mode==100){
			if (c=='\n') mode =0;
		}
	}
	printf("\nentry point %p\n",p);
	printf("bye\n");
	asm volatile("mv ra,%0" : : "r" (p));
	asm volatile("ret");
}

