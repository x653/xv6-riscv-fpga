/*
SPI - Serial Peripherial Interface

Provides memory mapped registers to control SPI connection to SD-Card
Base address: 0x10001000

RW:     Write to RW to send 8 bits to SDO with the appropriate SCK pulses
        Read from RW the last byte received at SDI
CTR:    Control the control wires:
        CTR[0] = CSN: Chip Select not
		CTR[1] = SCK: serial clock

register  offset  size       reset     
-----------------------------------------
RW        0x00    8 bits    0x00      
CTR       0x04    8 bits    0x00
*/
#include "printf.h"
#define DISK0 0x10001000
#define Reg(reg) ((volatile unsigned char *)(DISK0 + reg))

#define RW  0                 // read and write byte
#define CTR 4                 // control lines
#define SS  1<<0
#define SCK 1<<1

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// holds the CRC16 check sum
int crc;

// calculates CRC16 check sum
void crc16(unsigned char c){
	crc^=c<<8;
    for (int j = 0; j < 8; j++)
    {
		crc<<=1;
		if (crc &(1<<16)) {
			crc ^= 0x11021;
		}
    }
}

// get the time from register mtime
unsigned long long mtime(){
	unsigned int h1,l,h2;
  	asm volatile("csrr %0, 0xC80" : "=r" (h1) );
  	asm volatile("csrr %0, 0xC00" : "=r" (l) );
  	asm volatile("csrr %0, 0xC80" : "=r" (h2) );
	if (h1!=h2) return mtime();
	return (((unsigned long long)h1) << 32) | l;
}

// sleep given time t in microseconds
void sleep(unsigned int dt){
	dt = dt * 32;
	unsigned long long t = mtime();
	while (mtime() - t < dt);
}

// writes byte c to SPI and return the received byte
unsigned char spi_rw(unsigned char c)
{
	WriteReg(RW, c);	
	return ReadReg(RW);
}

// read 32 bit word from SPI
unsigned int spi_r(){
	unsigned int r=0;
	for (int i=0;i<4;++i){
		r=(r<<8)|spi_rw(255);
	}
	return r;
}

// sends command with 4 args and crc to SD-Card and returns response
char spi_cmd(char cmd,char arg0, char arg1, char arg2, char arg3, char crc){
	WriteReg(RW,255);
	WriteReg(RW,64|cmd);
	WriteReg(RW,arg0);
	WriteReg(RW,arg1);
	WriteReg(RW,arg2);
	WriteReg(RW,arg3);
	WriteReg(RW,crc);
	
	int i=0;
	while((i<10) && (spi_rw(255)==255)) ++i;
	//printf("CMD%x %p -> %d\n",cmd,arg0<<24|arg1<<16|arg2<<8|arg0,ReadReg(RW));
	return ReadReg(RW);
}

// initialises the SD-Card
int spi_init(){
	//printf("start init SD-Card\n");
	sleep(1000);				//wait >= 1ms
	WriteReg(CTR,1);			//CS high
	for (int i=0;i<100;++i){	//100 pulses on SCK with CS high
		sleep(1);
		WriteReg(CTR,3);
		sleep(1);
		WriteReg(CTR,1);
	}
	sleep(1);
	WriteReg(CTR,0);			//CS low################

	spi_cmd(0,0,0,0,0,149);
	spi_cmd(8,0,0,1,170,135);
	spi_r();
	
	for (int i=0;i<10;i++){
		sleep(100000);
		spi_cmd(55,0,0,0,0,0);
		spi_cmd(41,0x40,0,0,0,0);
		if (ReadReg(RW) == 0) break;
	}
	spi_cmd(58,0,0,0,0,0);
	int ocr=spi_r();
	if (~(ocr&(1<<30)))
		spi_cmd(16,0,0,2,0,0);
	return 0;
}

// reads a specific block of 512 bytes from SD-Card into buffer
int spi_rb(unsigned int block, unsigned char* buf){
	int b1=(block >>24) & 0xff;
	int b2=(block >>16) & 0xff;
	int b3=(block >>8) & 0xff;
	int b4=block & 0xff;
	if (spi_cmd(17,b1,b2,b3,b4,0)==0){
		int i=0;
		while((i<100000) && (spi_rw(255)==255)) ++i;
		crc = 0x0000;
		for (int i=0;i<512;++i){
			buf[i] = spi_rw(255);
			crc16(buf[i]);
		}
		crc16(spi_rw(255));
		crc16(spi_rw(255));
	}
	return (crc);
}

