#include "stdio.h"
#include <stdarg.h>

char digits[] = "0123456789abcdef";

void
printint(int xx, int base)
{
  char buf[16];
  int i;
  unsigned int x;
  int sign;
  if((sign = (xx < 0)))
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

void
printptr(unsigned int x)
{
  int i;
  putchar('0');
  putchar('x');
  for (i = 0; i < 8; i++, x <<= 4)
    putchar(digits[x >> (sizeof(int) * 8 - 4)]);
}

void
printbyte(unsigned char x)
{
  putchar(digits[x >> 4 & 0x0f]);
  putchar(digits[x & 0x0f]);
}

// Print to the console. only understands %b, %p, %s.
void
printf(char *fmt, ...)
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

