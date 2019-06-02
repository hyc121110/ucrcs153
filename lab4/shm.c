#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

// lab4
int shm_open(int id, char **pointer) {
  //you write this
  
  // initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  int i;
  // check page i is case 1 or case 2.
  int emptypage = -1; // index of the empty page
  int found = 0; // flag t
  int page_index = 100;
  for (i = 0; i < 64; i++) {
    if(shm_table.shm_pages[i].id == id){
      found = 1;
      page_index = i;
      break;
    }
    // empty page
    if(shm_table.shm_pages[i].id == 0 && emptypage == -1){
        emptypage = i;
    }
  }
  
  struct proc *p = myproc();
  // case 1: id found
  if (found == 1 && page_index < 64) {
    // 1st param: pgdir
    // 2nd param: va
    // 3rd param: size
    // 4th param: pa
    // 5th param: perm
    uint va = PGROUNDUP(p->sz);
    mappages(p->pgdir, (void*)va, PGSIZE, V2P(shm_table.shm_pages[page_index].frame), PTE_W|PTE_U);
    *pointer = (char*)va;
    shm_table.shm_pages[page_index].refcnt++;
    release(&(shm_table.lock));
    return va;
  }
  
  // case 2: id not found 
  // if emptypage == -1 -> no empty page
  // need check emptypage != -1 to allocate page
  else if (emptypage != -1) {
    // need to know index of empty page
    page_index = emptypage;
    // initialize its id to the id passed to us
    shm_table.shm_pages[page_index].id = id; 
    // We then kalloc a page and store its address in frame (we got our physical page).
    shm_table.shm_pages[page_index].frame = kalloc();
    memset(shm_table.shm_pages[page_index].frame, 0, PGSIZE);
    // set recnt to 1
    shm_table.shm_pages[page_index].refcnt = 1;
    // similar to case 1
    uint va = PGROUNDUP(p->sz);
    mappages(p->pgdir, (void*)va, PGSIZE, V2P(shm_table.shm_pages[page_index].frame), PTE_W|PTE_U);
    *pointer = (char*)va;
    release(&(shm_table.lock));
    return va;
  }
  release(&(shm_table.lock));
  return 0; //added to remove compiler warning -- you should decide what to return
}

// lab4
int shm_close(int id) {
  //you write this too!
  acquire(&(shm_table.lock));
  int page_index = 100;
  int i;
  for (i = 0; i< 64; i++) {
    if(shm_table.shm_pages[i].id == id){
      page_index = i;
      break;
    }
  }

  if (page_index < 64) {
    // decrement refcnt
    int cnt = shm_table.shm_pages[page_index].refcnt--;
    
    // if it reaches zero, then it clears the shm_table. 
    if (cnt == 0) {
      shm_table.shm_pages[i].id = 0;
      shm_table.shm_pages[i].frame = 0;
    }
  }
  release(&(shm_table.lock));
  return 0; //added to remove compiler warning -- you should decide what to return
}
