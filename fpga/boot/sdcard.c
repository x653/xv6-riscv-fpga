int crc;
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
	volatile char *p = (char*)0x20000000;
	*p = c;
	return *p;
}

char spi_cmd(char cmd,char arg0, char arg1, char arg2, char arg3, char crc){
	*(volatile char*)0x20000000 = 255;
	*(volatile char*)0x20000000 = 64|cmd;
	*(volatile char*)0x20000000 = arg0;
	*(volatile char*)0x20000000 = arg1;
	*(volatile char*)0x20000000 = arg2;
	*(volatile char*)0x20000000 = arg3;
	*(volatile char*)0x20000000 = crc;
	int i=0;
	while((i<10) && (spi_rw(255)==255)) ++i;
	return *(volatile char*)0x20000000;
}

unsigned int spi_r(){
	unsigned int r=0;
	for (int i=0;i<4;++i){
		r=(r<<8)|spi_rw(255);
	}
	return r;
}

int spi_init(){
	sdsleep(1000);
	*(volatile char*)0x20000004 = 1;
	for (int i=0;i<100;++i){
		sdsleep(1);
		*(volatile char*)0x20000004 = 3;
		sdsleep(1);
		*(volatile char*)0x20000004 = 1;
	}
	sdsleep(1);
	*(volatile char*)0x20000004 = 2;

	spi_cmd(0,0,0,0,0,149);
	spi_cmd(8,0,0,1,170,135);
	spi_r();
	for (int i=0;i<10;i++){
		sdsleep(100000);
		spi_cmd(55,0,0,0,0,0);
		spi_cmd(41,0x40,0,0,0,0);
		if (*(volatile char*)0x20000000 == 0) break;
	}
	spi_cmd(58,0,0,0,0,0);
	int ocr=spi_r();
	if (~(ocr&(1<<30)))
		spi_cmd(16,0,0,2,0,0);
	return 0;
}
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

