#include <stdio.h>
#include <stdarg.h>
unsigned char mem[512*1024];

char toHex(char c){
	if (c>='0' & c<='9') return c-'0';
	if (c>='a' & c<='f') return c-'a'+10;
	return -1;
}

static char digits[] = "0123456789abcdef";

static void
printint(int xx, int base)
{
  char buf[16];
  int i;
  unsigned int x;
  int sign;
  if(sign = xx < 0)
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    putchar(buf[i]);
}
static void
printptr(unsigned int x)
{
  int i;
  putchar('0');
  putchar('x');
  for (i = 0; i < 8; i++, x <<= 4)
    putchar(digits[x >> (sizeof(int) * 8 - 4)]);
}

static void
printbyte(unsigned char x)
{
  putchar(digits[x >> 4 & 0x0f]);
  putchar(digits[x & 0x0f]);
}

// Print to the console. only understands %b, %p, %s.
void
n_printf(char *fmt, ...)
{
  va_list ap;
  int i, c;
  char *s;

  va_start(ap, fmt);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      putchar(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'b':
      printbyte(va_arg(ap, int));
      break;
    case 'd':
      printint(va_arg(ap, int),10);
      break;
    case 'x':
      printint(va_arg(ap, int),16);
      break;
    case 'p':
      printptr(va_arg(ap, int));
      break;
    case 's':
      if((s = va_arg(ap, char*)) == 0)
        s = "(null)";
      for(; *s; s++)
        putchar(*s);
      break;
    case '%':
      putchar('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      putchar('%');
      putchar(c);
      break;
    }
  }

}

void dump(int p,int len){
	n_printf("Hex dump of section %p (%d bytes)\n",p,len);
	for (int i=0;i<len;i+=16){
		n_printf("  %p ",p+i);
		for (int ii=0;ii<16;ii++){
			n_printf("%b",mem[p+i+ii-0x80000000]);
			if (!(~ii & 0x3)) n_printf(" ");
		}
		for (int ii=0;ii<16;ii++){
			if ((mem[p+i+ii-0x80000000]>=32) & (mem[p+i+ii-0x80000000]<127)) putchar(mem[p+i+ii-0x80000000]);
			else putchar('.');
		}
		n_printf("\n");
	}
}

void main(void)
{
	n_printf("\nRISC-V\n");
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
			else {mode=4;x|=toHex(c);mem[p+cc-0x080000000]=x;++cc;}
		}else if (mode==42){
			if (c=='\n') {dump(p,cc);mode=0;}
			else {cc<<=4;cc|=toHex(c);}
		}else if (mode==43){
			mode=-1;
		}else if(mode==100){
			if (c=='\n') mode =0;
		}
	}
	n_printf("\nentry point %p\n",p);
	n_printf("bye\n");
//	asm volatile("mv ra,%0" : : "r" (p));
//	asm volatile("ret");
}

