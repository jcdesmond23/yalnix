/*
 * Tucker Simpson and Jack Desmond
 * CS58
 * 01/31/24
 * yalnix.c
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <yalnix.h>
#include <ylib.h>
#include <ykernel.h>
#include <yuser.h>
#include <hardware.h>
#include <ctype.h>
#include <sys/types.h>

////////////////////////// FUNCTION HEADERS //////////////////////////

void TrapKernel(UserContext* context);
void TrapClock(UserContext* context);
void TrapUnhandled(UserContext* context);
void DoIdle(void);

////////////////////////// DATA STRUCTURES //////////////////////////

// Remember to flush the TLB every time we switch processes.

typedef struct Pcb {
  UserContext uCtxt;
  KernelContext kKtxt;
  int baseStackFrame;
  int topStackFrame;
  int pageTableBase;
  int limitPageTable;
} Pcb_t;
////////////////////////// GLOBALS //////////////////////////

// Total clock ticks
unsigned long CLOCK;
// Pointer to current PCB
Pcb_t* CURR_PCB;
// Flag indicating whether virtual memory is enabled
int VIRTUAL_MEMORY_ENABLED = 0;
int BRK;    // first unused page
// Ready Queue
Pcb_t* READY_QUEUE_HEAD;
Pcb_t* READY_QUEUE_TAIL;

typedef void (*handlerFunction)(UserContext*);

// PCB stuff
// we want to make a large table indexed by pid, mapped to Pcbs
Pcb_t* pcbTable[1000];

u_int8_t* freeFrames;
pte_t* pageTable;

// struct pfn_node {
//   struct pfn_node* next;
//   struct pfn_node* prev;
//   u_long pfn     : 24;
// };

// typedef struct pfn_node pfn_node_t;


////////////////////////// Booting //////////////////////////

void
KernelStart(char *cmd_args[],  unsigned int pmem_size, UserContext *uctxt) {
  TracePrintf(0, "Hello world.\n");

  BRK = _orig_kernel_brk_page;

  // Set up a bit vector to track free frames

  // Calculate the number of frames available.
  int num_phys_frames = pmem_size / PAGESIZE;
  TracePrintf(0, "Number of frames: %d\n", num_phys_frames);
  TracePrintf(0, "PMEM_BASE: %d\n", PMEM_BASE);

  // Initialize bit array

  // Do something like this later, maybe. But for now just use an int array.
  // int sizeOfBitMap = (num_phys_frames + 31) / 32;
  // u_int32_t* freeFrames = calloc(sizeOfBitMap, sizeof(u_int32_t));
  // if (freeFrames == NULL) {
  //   TracePrintf(0, "Error initializing free frame bit vector.\n");
  // }
  
  // Initialize "bit" vector for free frame tracking
  // freeFrames[pfn] = 0 means the frame is free, = 1 means the frame is taken
  freeFrames = calloc(num_phys_frames, sizeof(u_int8_t));

  // Mark every page below _first_kernel_text_page as used so we don't touch it
  for (int i = 0; i < _first_kernel_text_page; i++) {
    freeFrames[i] = 1;
  }
  
  // set up the page table.

  pageTable = malloc(num_phys_frames * sizeof(pte_t));
  if (pageTable == NULL) { }
  // how many entries in the page table? --> num_phys_frames?
  for (int i = 0; i < num_phys_frames; i++) {
    pageTable[i].pfn = 0;
    pageTable[i].valid = 0;
    pageTable[i].prot = 0;
  }

  // do we just map everything from [_first_kernel_text_page, _orig_kernel_brk_page) to physical mem?
  for (int i = _first_kernel_text_page; i < _orig_kernel_brk_page; i++) {
    pageTable[i].pfn = i;
    freeFrames[i] = 1;
    pageTable[i].valid = 1;
    if (i < _first_kernel_data_page) {
      pageTable[i].prot |= PROT_READ;
      pageTable[i].prot |= PROT_EXEC;
    }
    else {
      pageTable[i].prot |= PROT_ALL;
    }
  }

  for (int i = KERNEL_STACK_BASE >> PAGESHIFT; i < KERNEL_STACK_LIMIT >> PAGESHIFT; i++) {
    pageTable[i].pfn = i;
    freeFrames[i] = 1;
    pageTable[i].valid = 1;
    pageTable[i].prot |= PROT_ALL;
  }

  for (int i = VMEM_1_BASE >> PAGESHIFT; i < VMEM_1_LIMIT >> PAGESHIFT - 1; i++) {
    pageTable[i].pfn = i;
    freeFrames[i] = 1;
    pageTable[i].valid = 0;
    pageTable[i].prot = 0;
  }
  int idleStackPage = VMEM_1_LIMIT >> PAGESHIFT - 1;
  freeFrames[idleStackPage] = 1;
  pageTable[idleStackPage].pfn = idleStackPage;
  pageTable[idleStackPage].valid = 1;
  pageTable[idleStackPage].prot |= PROT_ALL;
  WriteRegister(REG_PTBR0, VMEM_0_BASE);
  WriteRegister(REG_PTLR0, VMEM_0_SIZE >> PAGESHIFT);
  WriteRegister(REG_PTBR1, VMEM_1_BASE);
  WriteRegister(REG_PTLR1, VMEM_1_SIZE >> PAGESHIFT);

  // Initialize init PCB
	// Initialize interrupt handler vector and write to register
  typedef void (*handlerFunction)(UserContext*);
  handlerFunction* handlerTable = malloc(sizeof(handlerFunction) * TRAP_VECTOR_SIZE);
  handlerTable[TRAP_KERNEL] = &TrapKernel;
  handlerTable[TRAP_CLOCK] = &TrapClock;
  for (int i = TRAP_CLOCK + 1; i < TRAP_VECTOR_SIZE; i++) {
    handlerTable[i] = &TrapUnhandled;
  }
  WriteRegister(REG_VECTOR_BASE, (unsigned int) &handlerTable);

  if (BRK > _orig_kernel_brk_page) {
    int newKernelBrkPage = UP_TO_PAGE(BRK);
    for (int i = _orig_kernel_brk_page; i <= newKernelBrkPage; i++) {
      pageTable[i].pfn = i;
      pageTable[i].valid = 1;
      pageTable[i].prot |= PROT_ALL;
    }
  }

  // Write to the Virtual Memory Enable Register to enable virtual memory
  WriteRegister(REG_VM_ENABLE, 1);
  Pcb_t idlePCB;
  idlePCB.uCtxt = *uctxt;
  idlePCB.uCtxt.pc = &DoIdle;
  idlePCB.uCtxt.sp = (void*) VMEM_1_LIMIT;
  uctxt->pc = idlePCB.uCtxt.pc;
  uctxt->sp = (void*) VMEM_1_LIMIT - 1;
  TracePrintf(1, "Leaving KernelStart!\n");
} 

int 
SetKernelBrk(void * addr) {
  int addrPage = UP_TO_PAGE(addr);
  if (!VIRTUAL_MEMORY_ENABLED) {
    if (addrPage >= BRK) {
      BRK = addrPage + 1;
      return 0;
    }
    // if we're malloc'ing into the stack
    if (addrPage >= KERNEL_STACK_BASE >> PAGESHIFT) {
      return ERROR;
    }
  }
  else {
    // figure out later.
  }
  return 0;
}

////////////////////////// INTERRUPTION HANDLERS //////////////////////////

void 
TrapKernel(UserContext* context) {
	// Switch statements by call code so the right syscall is executed
  // Set reg[0] of context with the return value of the syscall made
  // Trace print the hex code of the call
  TracePrintf(1, "Kernel trap!");
}

void
TrapClock(UserContext* context) {
	// If ready queue isn't empty
  	// Context switch to the next process in the queue ??????????
  // Else
  	// Dispatch the idle process
  TracePrintf(1, "Clock trap!");
}

void 
TrapUnhandled(UserContext* context) {
  // Unhandled trap handler
  TracePrintf(1, "Unhandled trap!");
}

void
DoIdle(void) {
  while(1) {
    TracePrintf(1,"DoIdle\n");
    Pause();
  } 
}