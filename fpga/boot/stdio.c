/*
stdio - provides standard input output over uart

UART0
Provides memory mapped registers to control UART connection
Base address: 0x10000000

register  offset  size       reset     
-----------------------------------------
RW        0x00    8 bits    0x00      
CTR       0x04    8 bits    0x00
*/
#define UART0 0x10000000
#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

#define RHR  0                 // receive holding register
#define THR  0                 // transmit holding register
#define LCR  3				   // line control register
#define LCR_BAUD_LATCH (1<<7)	// special mode to set baud rate
#define LSR  5					// line status register
#define LSR_RX_READY (1<<0)			// transmit holding register empty
#define LSR_THE	(1<<5)			// transmit holding register empty
#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

void uart_init(){
	WriteReg(LCR,LCR_BAUD_LATCH);
	WriteReg(0, 32000000/115200 & 0xff);
	WriteReg(1, 32000000/115200 >> 8);
	WriteReg(LCR, 0x00);
}

void putchar(unsigned char c)
{	
	while ((ReadReg(LSR) & LSR_THE) == 0);//wait for THE (Transmit Holding empty)
	WriteReg(THR,c);
}

int getchar()
{
	while ((ReadReg(LSR) & LSR_RX_READY) ==0);//wait for DR (data ready) 
	return ReadReg(RHR);
}

