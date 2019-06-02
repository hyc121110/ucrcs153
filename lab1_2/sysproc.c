#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  int exitStatus;

  // added by Evan Chan lab1a
  argint(0, &exitStatus);
  exit(exitStatus);

  // exit(0);
  return 0;  // not reached
}

int
sys_wait(void)
{
  int *exitStatus;
  // added by Evan Chan lab1c
  argptr(0, (char **) &exitStatus, sizeof(int*));
  return wait(exitStatus);
}

// added by Evan Chan lab1c
int
sys_waitpid(void)
{
  int pid;
  int* exitStatus;
  int options;

  argint(0, &pid);
  argptr(1, (char **) &exitStatus, sizeof(int*));
  argint(2, &options);

  return waitpid(pid, exitStatus, options);
}

// added by Evan Chan lab2
int
sys_setpriority(void)
{
  int val;
  argint(0, &val);
  setpriority(val);
  return 0;
}

// added by Evan Chan lab2
int
sys_getpriority(void)
{
  return getpriority();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
