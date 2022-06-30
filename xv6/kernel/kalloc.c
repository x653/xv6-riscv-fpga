// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
//int n_kalloc=0;
void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.
//extern int paging;
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  //printf("kinit end=%p PHYSTOP=%p\n",end,PHYSTOP);
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint32)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint32)pa % PGSIZE) != 0 || (char*)pa < end || (uint32)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  //printf("kalloc %d\n",++n_kalloc);
	//printf("acquire %p %d %d\n",&kmem.lock,kmem.lock.locked,kmem.lock.cpu);
  acquire(&kmem.lock);
  r = kmem.freelist;
	//printf("r %p\n",(int)r);
	//printf("paging= %d\n",paging);
  //if(paging)  printf("translate %p -> %p\n",r,kvmpa((int)r));
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);
	//printf("release %p\n",&kmem.lock);
	//printf("r= %p\n",r);
	//printf("paging= %d\n",paging);
  //if(paging)  printf("translate %p -> %p\n",r,kvmpa((int)r));

	//printf("PGSIZE= %d\n",PGSIZE);
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  //printf("kalloc return  %p\n",r);
  return (void*)r;
}
