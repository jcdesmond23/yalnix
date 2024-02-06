// /*
//  * Tucker Simpson and Jack Desmond
//  * CS58
//  * 01/31/24
//  * yalnix.c
//  * 
//  */

// #include <ykernel.h>


// ////////////////////////// DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\

// // Remember to flush the TLB every time we switch processes.

// struct Pcb {
//   int pid;
//   UserContext uCtxt;
//   // UserSpace (address space)
//   KernelContext kKtxt;
//   Pcb* livingChildrenHead;
//   int brk;
//   int parentPid;
//   unsigned alive	: 1
//   int exitStatus;
//   Pcb* next;
//   Pcb* prev;
// 	unsigned waitingPid	: 1
//   unsi
//   Pcb* deadChildrenHead;
// }

// struct Lock {
//   int id;
//   int ownerPID;
//   Pcb* waitingHead;
//   Pcb* waitingTail;
// }

// struct Cvar {
//   int id;
//   Pcb* waitingHead;
//   Pcb* waitingTail;
// }

// struct Pipe {
//   int id;
//   int ownerID;
//   Pcb* blockedReaders;
//   char* contents;
//   int plen;
//   char* nextToRead;
//   char* nextToWrite;
// }

// ////////////////////////// GLOBALS \\\\\\\\\\\\\\\\\\\\\\\\\\
// // Total clock ticks
// unsigned long CLOCK;
// // Pointer to current PCB
// Pcb* CURR_PCB;
// // Flag indicating whether virtual memory is enabled
// int VIRTUAL_MEMORY_ENABLED;
// int BRK;
// // Ready Queue
// Pcb* READY_QUEUE_HEAD;
// Pcb* READY_QUEUE_TAIL;
// // PCB stuff
// // we want to make a large table indexed by pid, mapped to Pcbs
// Pcb* pcbTable[1000];


// ////////////////////////// Booting \\\\\\\\\\\\\\\\\\\\\\\\\\
// void
// KernelStart(char *cmd_args[],  unsigned int pmem_size, UserContext *uctxt) {
// 	// Set up a bit vector to track free frames
//   // Initialize init PCB
// 	// Initialize interrupt handler bit vector and write to register
// 	// Set up the initial Region 0 page table
// 	// Set up Region 1 page table for idle
// 	// Has one valid page for idle’s user stack
// 	// Write to the page table registers to tell the hardware where the page tables are located in memory
//   // Write to the Virtual Memory Enable Register to enable virtual memory
//   // initialize init process
//   // initialize idle process
// }

// ////////////////////////// Booting \\\\\\\\\\\\\\\\\\\\\\\\\\

// int 
// SetKernelBrk(void * addr) {
// 	// If not enough memory is available
//   	// return ERROR
//   // If addr is not valid
//   	// return ERROR
//   // Call helper function to round addr to nearest page
//   // global BRK = rounded addr
//   // Return 0
// }

// ////////////////////////// SYSCALLS \\\\\\\\\\\\\\\\\\\\\\\\\\

// int
// fork(void) {
// 	// Get PID for child
//   // Initialize PCB for child
//   // Set PCB_table[pid] = childPCB
//   // Copy memory image of the parent process to the child process (set up page table)
//   // Mark parent in childPCB = currProcess global
//   // Link the child into the currentProcess's living children LL
  
//   // Link child into ready queue with current execution context of parent
  
//   // Child process should start running right here
//   // if currProcess pid = childPid
//   		// return 0
    
//   // return child Pid
//   // Will this successfully return two different things?
// }

// int
// Exec(char *filename, char **argvec) {
// 	// load program from filename
//   // if error
//   	// if calling process exists return ERROR
//     // else terminate the program
//     // Do we need these^^ or will it be determined by our design
//   // copy the arguments into the program's address space
//   // move the new program onto the ready queue
//   // terminate the calling process? or just overwrite the calling process
// }

// void
// Exit(int status) {
// 	// free the kernelStack, kernelContext, UserContext, and UserSpace
    
//   // If currProcess = initPCB
//   		// Halt everything ... and clean up?

//   // if it has child processes
//   		// go through the PCB entries of each living child
//         // if the child is not dead, set parent = NULL
//         	/* 
//         	 * Reason for this is we don't want to put a new process at the old
//            * parent's PID and get weird behavior with the child process
//            * not realizing that its parent is dead
//            */
//       // go through the deadChildren linked list
//       	// free remaining resources for each dead child
  
//   // if currentProcess's parent is not alive (parent = NULL)
//   		// free all resources associated with the program (subroutine)
//       		// including freeing page table
//       // terminate the program
//       // done.
    
//   // else
//   		// Set current process's PCB's dead = 1
//   		// Set current process's PCBs' exit status = status
//       // append the pcb to the deadChildren LL
//       // remove the child from the parent's living children list
//       // if waitingPID in parent PCB = true
//       	// set waiting PID in parent PCB to false
// 	      // move the parent to the ready queue
// }

// int
// Wait(int *status_ptr) {
// 	// if currPCB's deadChildren LL is not set to NULL
//   		// pop one of the PCBs from this LL
    
//     	// if status_ptr isn't NULL
//     			// put the exit status from popped PCB into the int at status_ptr
        
//       // save the pid from the popped PCB
//       // free everything associated with the pid
//       // return child's pid

//   // else if no living children
//   		// return ERROR
  
//   // else
//   	// set curr_pcb waitingPID = true
    
//       /* child will put the parent back onto the ready queue... i think we can 
//        * pick up running right here */
     
//     // pop one of the PCBs from deadChildren LL (if it's empty we have a bug)
    
//     	// if status_ptr isn't NULL
//     			// put the exit status from popped PCB into the int at status_ptr
        
//       // save the pid from the popped PCB
//       // free everything associated with the pid
//       // return child's pid
// }

// int
// GetPid(void) {
// 	// return PID of runningPCB
// }

// int
// Brk(void *addr) {
// 	// If not enough memory is available
//   	// return ERROR
//   // If addr is not valid
//   	// return ERROR
//   // Call helper function to round addr to nearest page
//   // BRK in user heap = rounded addr
//   // Return 0
// }

// int
// Delay(int clock_ticks) {
// 	// if clock_ticks == 0
//   	// return 0
//   // elif clock_ticks < 0
//   	// return ERROR
//   // start_clock = current clock value in global variable
//   // While stack_clock + clock_ticks > global clock value
//   	// do nothing
//   // return 0
// }


// int 
// TtyRead(int tty_id, void *buf, int len) {
// 	// Read next line from terminal with tty_id
//   // If there is no next line of input
//   	// Add PCB to waiting queue
//   // If the length of the next line is less than or equal to len
//   	// Copy the entire line to buf and return the length of the line copied
//   // If the length of the next line is greater than len
//   	// Copy the first len bytes to buf and return len
// }

// int 
// TtyWrite(int tty_id, void *buf, int len) {
// 	// Write as many bytes to buffer as possible
//   // if failed to write
//   	// return ERROR
//   // if len bytes can be written
//   	// return len
//   // if less than len bytes can be written 
//   	// add the pcb to waiting queue
// }

// int 
// PipeInit(int *pipe_idp) {
// 	// Get pipe ID
// 	// Allocate memory for pipe
// 	// Put pipe on the kernel heap

// }

// int 
// PipeRead(int pipe_id, void *buf, int len) {
// 	// If current process’s PID != pipe owner PID and the current process’s parent PID != pipe owner PID
// 		// Return ERROR
// 	// If pipe is empty
// 		// Move caller to the waiting queue
// 	// If pipe has plen <= len unread bytes
// 		// Copy the contents to the buf
// 		// Free the contents of the pipe
// 		// Update plen
// 		// Return
// 	// If pipe has plen > len unread bytes
// 		// Copy the first len bytes to buff
// 		// Free the first len bytes from pipe contents
// 		// Update plen = plen - len
// 	// Return
// }

// int 
// PipeWrite(int pipe_id, void *buf, int len) {
// 	// If current process’s PID != pipe owner PID and the current process’s parent PID != pipe owner PID
// 		// Return ERROR
// 	// Append the bytes starting at buf to the end of the pipe contents
// 	// If error:
// 		// Return ERROR
// 	// Return number of bytes written
// }

// int
// LockInit(int *lock_idp) {
// 	// Initialize a lock struct
//   // If ERRROR
// 		// return ERROR
//   // Save lock id to memory at lock_idp
// 	// return 0

// }

// int 
// Acquire(int lock_id) {
// 	// Add PCB to the waiting queue if in use
//   // Add PID to the 
// 	// If free then set the locks owner
// }

// int 
// Release(int lock_id) {
// 	// If lock owner PID is not caller PID
//   	// return ERROR
//   // If there are PCB's waiting to acquire in the lock's queue
//   	// Pop from the lock's queue and update the lock's owner
//     // Dispatch the process popped from the queue by adding it to the ready queue
//   // Else
//   	// free the lock
//   // return 0
// }

// int 
// CvarInit(int *cvar_idp) {
// 	// Initialize a new Cvar object
//   // If ERROR
//   	// return ERROR
//   // Save the new Cvar id to memory at cvar_idp
// 	// return 0
// }

// int 
// CvarSignal(int cvar_id) {
// 	// Pop the next process in the CVAR waiting queue
//   // Move the PCB to the ready queue to dispatch it
//   // return 0
// }

// int 
// CvarBroadcast(int cvar_id) {
// 	// Add all processes in the CVAR waitng queue to the kernel ready queue
//   // return 0
// }

// int 
// CvarWait(int cvar_id, int lock_id) {
// 	// Call release on the lock with lock_id 
//   // Add the process to the queue Cvar object with id == cvar_id
// }

// int
// Reclaim(int id) {
// 	// Free the memory used for lock, cvar, or pipe with the id
// }

// ////////////////////////// INTERRUPTION HANDLERS \\\\\\\\\\\\\\\\\\\\\\\\\

// void 
// TrapKernel(UserContext* context) {
// 	// Switch statements by call code so the right syscall is executed
//   // Set reg[0] of context with the return value of the syscall made
//   // Trace print the hex code of the call
//   TracePrintf(1, "Kernel trap!");
// }

// void
// TrapClock(UserContext* context) {
// 	// If ready queue isn't empty
//   	// Context switch to the next process in the queue ??????????
//   // Else
//   	// Dispatch the idle process
//   TracePrintf(1, "Clock trap!");
// }

// void
// TrapIllegal() {
// 	// print insightful error message
//   // call Exit with value ERROR
// }

// void 
// TrapMemory() {
// 	// if addr in UserContext is above current BRK value but below the currently allocated memory for the stack
// 		// Grow the stack to allow for the addr in UserContext
//   // print insightful error message
//   // call Exit with value ERROR
// }

// void
// TrapMath() {
// 	// print insightful error message
//   // call Exit with value ERROR
// }

// void
// TrapTtyTransmit() {
// 	// call TtyTransmit machine function
// }

// void 
// TrapTtyRecieve() {
// 	// call TtyRecieve machine function
// }

// void 
// TrapUnhandled() {
//   // Unhandled trap handler
//   TracePrintf(1, "Unhandled trap!");
// }

// void
// DoIdle(void) {
//   while(1) {
//     TracePrintf(1,"DoIdle\n");
//     Pause();
//   } 
// }