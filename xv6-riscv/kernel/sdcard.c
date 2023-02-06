
#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

struct spinlock sdlock;
unsigned long long sdtime(){
	unsigned int h1,l,h2;
  	asm volatile("csrr %0, 0xC80" : "=r" (h1) );
  	asm volatile("csrr %0, 0xC00" : "=r" (l) );
  	asm volatile("csrr %0, 0xC80" : "=r" (h2) );
	if (h1!=h2) return sdtime();
	return (((unsigned long long)h1) << 32) | l;
}

void sdsleep(unsigned int d){
	unsigned long long dt = d*50;
	unsigned long long t = sdtime();
	while (sdtime() - t < dt);
}
char spi_rw(char c)
{	
	volatile char *p = (char*)0x10001000;
	*p = c;
	return *p;
}

int getCRC16(unsigned char message[], int length)
{
  int i, j, crc = 0x0000;
 
  for (i = 0; i < length; i++)
  {
	crc^=message[i]<<8;
    for (j = 0; j < 8; j++)
    {
		crc<<=1;
      if (crc &(1<<16)) {
		crc ^= 0x11021;
		}
    }
  }
  return crc;
}
char spi_cmd(char cmd,char arg0, char arg1, char arg2, char arg3, char crc){
	*(volatile char*)0x10001000 = 255;
	*(volatile char*)0x10001000 = 64|cmd;
	*(volatile char*)0x10001000 = arg0;
	*(volatile char*)0x10001000 = arg1;
	*(volatile char*)0x10001000 = arg2;
	*(volatile char*)0x10001000 = arg3;
	*(volatile char*)0x10001000 = crc;
	int i=0;
	while((i<10) && (spi_rw(255)==255)) ++i;
	return *(volatile char*)0x10001000;
}

unsigned int spi_r(){
	unsigned int r=0;
	for (int i=0;i<4;++i){
		r=(r<<8)|spi_rw(255);
	}
	return r;
}

int spi_init(){
  	initlock(&sdlock, "sd");
	//printf("spi_init");
	sdsleep(1000);
	//printf("sleep");
	*(volatile char*)0x10001004 = 1;
	for (int i=0;i<100;++i){
		sdsleep(1);
		*(volatile char*)0x10001004 = 3;
		sdsleep(1);
		*(volatile char*)0x10001004 = 1;
	}
	sdsleep(1);
	*(volatile char*)0x10001004 = 2;

	spi_cmd(0,0,0,0,0,149);
	spi_cmd(8,0,0,1,170,135);
	spi_r();
	for (int i=0;i<10;i++){
		sdsleep(100000);
		spi_cmd(55,0,0,0,0,0);
		spi_cmd(41,0x40,0,0,0,0);
		if (*(volatile char*)0x10001000 == 0) break;
	}
	spi_cmd(58,0,0,0,0,0);
	int ocr=spi_r();
	if (~(ocr&(1<<30)))
		spi_cmd(16,0,0,2,0,0);
	return 0;
}

void spi_rb(unsigned int block, unsigned char* buf){
 acquire(&sdlock);
	int b1=(block >>24) & 0xff;
	int b2=(block >>16) & 0xff;
	int b3=(block >>8) & 0xff;
	int b4=block & 0xff;
	if (spi_cmd(17,b1,b2,b3,b4,0)==0){
		int i=0;
		while((i<100000) && (spi_rw(255)==255)) ++i;
		for (int i=0;i<514;++i){
			buf[i] = spi_rw(255);
		}
	}
	if (getCRC16(buf,514)) panic("CRC16 error\n");
 release(&sdlock);
}

void spi_wb(unsigned int block, unsigned char* buf){
 acquire(&sdlock);
	int b1=(block >>24) & 0xff;
	int b2=(block >>16) & 0xff;
	int b3=(block >>8) & 0xff;
	int b4=block & 0xff;
	spi_cmd(24,b1,b2,b3,b4,0);
	int i=0;
	while(spi_rw(255)!=255){
		if (i>100000) panic("spi_wb1\n");
		i++;
	}
	spi_rw(254);
	for (int i=0;i<514;++i){
		spi_rw(buf[i]);
	}
	spi_rw(255);	
	i=0;
	while((spi_rw(255)!=255)){
		if (i>100000) panic("spi_wb2\n");
		i++;
	}
 release(&sdlock);
}
