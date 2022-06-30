#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "sdcard.h"
volatile static int started = 0;
//int paging=0;
// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    //printf("main: kinit OK\n");
    kvminit();       // create kernel page table
    //printf("main: kvminit OK\n");
    kvminithart();   // turn on paging
    //printf("main: kvminithart OK\n");
	//paging=1;
	//unsigned int p = TRAMPOLINE;
    //printf("main: translate %p -> %p\n",p,kvmpa(p));
    procinit();      // process table
    //printf("main: procinit OK\n");
    trapinit();      // trap vectors
    //printf("main: trapinit OK\n");
    trapinithart();  // install kernel trap vector
    //printf("main: trapinithart OK\n");
    plicinit();      // set up interrupt controller
    //printf("plicinit OK\n");
    plicinithart();  // ask PLIC for device interrupts
    //printf("plicinithart OK\n");
    binit();         // buffer cache
    //printf("binit OK\n");
    iinit();         // inode cache
    //printf("iinit OK\n");
    fileinit();      // file table
    //printf("fileinit OK\n");
	spi_init();
    //printf("spi_init OK\n");
    //virtio_disk_init(); // emulated hard disk
    //printf("vitiio_init OK\n");
    userinit();      // first user process
    //printf("userinit OK\n");
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }
  scheduler();        
}
