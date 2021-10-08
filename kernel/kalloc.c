// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};


struct {
  struct spinlock lock;
  struct run *freelist;
  int ref[PHYSTOP / PGSIZE];
} kmem;

int idx(uint64 pa){
  return pa / PGSIZE;
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void printFreeNum() 
{
  int num = 0;
    acquire(&kmem.lock);
    struct run *r  = kmem.freelist;
    while(r)
    {
      r=r->next;
      num++;
    }
    printf("FREE BLOCKS: %d\n", num);
  release(&kmem.lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

void kinc(void *pa)
{
  int index = idx((uint64)pa);

  acquire(&kmem.lock);
  kmem.ref[index]++;
  release(&kmem.lock);
}

// void kdec(void *pa)
// {
//   int index = idx((uint64)pa);

//   acquire(&kmem.lock);
//   kmem.ref[index]--;
//   if(kmem.ref[index] <= 0){
//     kfree(pa);
//   }
//   release(&kmem.lock);
// }

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  r = (struct run*)pa;

   int index = idx((uint64)pa);

  acquire(&kmem.lock);
  kmem.ref[index]--;
  if(kmem.ref[index]>0){
    release(&kmem.lock);
    return ;
  }
  release(&kmem.lock);

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);


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

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    memset((char*)r, 5, PGSIZE); // fill with junk
    kmem.ref[idx((uint64)r)] = 1;
  }
  release(&kmem.lock);
  return (void*)r;
}
