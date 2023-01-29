
void uart_init(){
	volatile char *p = (char*)0x10000000;
	*(p+3) = 1<<7;
	*(p) = (32000000/115200-1)&0xff;
	*(p+1) = (32000000/115200-1)>>8;
	*(p+3) = 0;
}

void putchar(char c)
{	
	volatile char *p = (char*)0x10000000;
	while (((*(p+5)) & 0x40) ==0);//wait for THE (Transmit Holding empty)
	*p = c;
}

int getchar()
{
	volatile char *p = (char*)0x10000005;
	while (((*p) & 0x01) ==0);//wait for DR (data ready) 
	int c = *(char*)0x10000000;
	return c;
}

