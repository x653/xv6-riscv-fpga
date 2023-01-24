// Saved registers for kernel context switches.
struct context {
  uint32 ra;
  uint32 sp;

  // callee-saved
  uint32 s0;
  uint32 s1;
  uint32 s2;
  uint32 s3;
  uint32 s4;
  uint32 s5;
  uint32 s6;
  uint32 s7;
  uint32 s8;
  uint32 s9;
  uint32 s10;
  uint32 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context scheduler;   // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// the sscratch register points here.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe {
  /*   0 */ uint32 kernel_satp;   // kernel page table
  /*   4 */ uint32 kernel_sp;     // top of process's kernel stack
  /*   8 */ uint32 kernel_trap;   // usertrap()
  /*  12 */ uint32 epc;           // saved user program counter
  /*  16 */ uint32 kernel_hartid; // saved kernel tp
  /*  20 */ uint32 ra;
  /*  24 */ uint32 sp;
  /*  28 */ uint32 gp;
  /*  32 */ uint32 tp;
  /*  36 */ uint32 t0;
  /*  40 */ uint32 t1;
  /*  44 */ uint32 t2;
  /*  48 */ uint32 s0;
  /*  52 */ uint32 s1;
  /*  56 */ uint32 a0;
  /*  60 */ uint32 a1;
  /*  64 */ uint32 a2;
  /*  68 */ uint32 a3;
  /*  72 */ uint32 a4;
  /*  76 */ uint32 a5;
  /*  80 */ uint32 a6;
  /*  84 */ uint32 a7;
  /*  88 */ uint32 s2;
  /*  92 */ uint32 s3;
  /*  96 */ uint32 s4;
  /* 100 */ uint32 s5;
  /* 104 */ uint32 s6;
  /* 108 */ uint32 s7;
  /* 112 */ uint32 s8;
  /* 116 */ uint32 s9;
  /* 120 */ uint32 s10;
  /* 124 */ uint32 s11;
  /* 128 */ uint32 t3;
  /* 132 */ uint32 t4;
  /* 136 */ uint32 t5;
  /* 140 */ uint32 t6;
};

enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  struct proc *parent;         // Parent process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // these are private to the process, so p->lock need not be held.
  uint32 kstack;               // Bottom of kernel stack for this process
  uint32 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // Page table
  struct trapframe *tf;        // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
};
