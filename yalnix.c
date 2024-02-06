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


////////////////////////// DATA STRUCTURES //////////////////////////

// Remember to flush the TLB every time we switch processes.

typedef struct Pcb {
  int pid;
  UserContext uCtxt;
  KernelContext kKtxt;
  struct Pcb* livingChildrenHead;
  int brk;
  int parentPid;
  unsigned alive	: 1;
  int exitStatus;
  struct Pcb* next;
  struct Pcb* prev;
	unsigned waitingPid	: 1;
  struct Pcb* deadChildrenHead;
} Pcb_t;
////////////////////////// GLOBALS //////////////////////////

// Total clock ticks
unsigned long CLOCK;
// Pointer to current PCB
Pcb_t* CURR_PCB;
// Flag indicating whether virtual memory is enabled
int VIRTUAL_MEMORY_ENABLED;
int BRK;
// Ready Queue
Pcb_t* READY_QUEUE_HEAD;
Pcb_t* READY_QUEUE_TAIL;

// PCB stuff
// we want to make a large table indexed by pid, mapped to Pcbs
Pcb_t* pcbTable[1000];


////////////////////////// Booting //////////////////////////

void
KernelStart(char *cmd_args[],  unsigned int pmem_size, UserContext *uctxt) {
  TracePrintf(0, "Hello world.\n");

  // Set up a bit vector to track free frames

  // Calculate the number of frames available.
  int num_phys_frames = pmem_size / PAGESIZE;
  TracePrintf(0, "Number of frames: %d\n", num_phys_frames);

  // Initialize an empty bit array of 0s of size num_frames, full of 0s.

  // Can we assume 4 bytes in an int? Or should we go about somehow else...
  // Could also go with the queue of free fame option..
  // 32 bits per int
  // So we need math.ceil(num_phys_frames / 32)?


  


  // Initialize init PCB
	// Initialize interrupt handler bit vector and write to register
	// Set up the initial Region 0 page table
	// Set up Region 1 page table for idle
	// Has one valid page for idle’s user stack
	// Write to the page table registers to tell the hardware where the page tables are located in memory
  // Write to the Virtual Memory Enable Register to enable virtual memory
  // initialize init process
  // initialize idle process
} 

int 
SetKernelBrk(void * addr) {
	// If not enough memory is available
  	// return ERROR
  // If addr is not valid
  	// return ERROR
  // Call helper function to round addr to nearest page
  // global BRK = rounded addr
  // Return 0
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
TrapUnhandled() {
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