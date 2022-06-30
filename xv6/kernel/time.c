#include "time.h"
unsigned long long time(){
	unsigned int h1,l,h2;
  	asm volatile("csrr %0, 0xC80" : "=r" (h1) );
  	asm volatile("csrr %0, 0xC00" : "=r" (l) );
  	asm volatile("csrr %0, 0xC80" : "=r" (h2) );
	if (h1!=h2) return time();
	return (((unsigned long long)h1) << 32) | l;
}

void sleep(unsigned int d){
	d=d*50;
	unsigned long long t = time();
	while (time() - t < d);
}
