// File: pcb.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "pcb.h"
#include <dos.h>
#include <stdio.h>
#include <iostream.h>

// Initializing static variables
ID PCB::threadID = 0;
PCB* PCB::running = NULL;
PCB* PCB::idle = NULL;
Deque<PCB*>* PCB::allPCB = new Deque<PCB*>();

// Size of one element on the stack
const int stackElementSize = sizeof(unsigned);

void PCB::initializeStack() {
	/* Initializes the stack with known size, and
	 * sets all stack-associated registers (ss, sp, bp)
	 * accordingly. To prevent memory leak in case of
	 * multiple calls, stack is first deleted to ensure
	 * clean slate (null pointer at the beginning).
	 */
	this->deleteStack();
	stack = new unsigned[stackSize];
#ifndef BCC_BLOCK_IGNORE
	stack[stackSize-1] = 0x200;					// PSWI <= 1
	stack[stackSize-2] = FP_SEG(PCB::wrapper);	// body function: SEG
	stack[stackSize-3] = FP_OFF(PCB::wrapper);	//              + OFF
												// AX...DX (8 entries)
	ss = FP_SEG(stack + stackSize-12);			// update thread's SS
	sp = FP_OFF(stack + stackSize-12);			// update thread's SP
#endif
	bp = sp;									// update thread's BP
}
// NOTICE - Reference: "v2_zad4.cpp".

void PCB::deleteStack() {
	/* Deletes the dynamically allocated stack.
	 * Stack is only implemented as a pointer
	 * to primitive data, so a simple delete
	 * operator call is sufficient for now.
	 */
	delete stack;
	stack = NULL;
}

PCB::PCB(StackSize stackSize, Time timeSlice, Thread* myThread) :
	id(++threadID),  stackSize(stackSize/stackElementSize), stack(NULL),
	ss(0), sp(0), bp(0), timeSlice(timeSlice), myThread(myThread),
	state(NEW), semaphoreTimeLeft(-1) {
	/* Constructor initializes fields related to: thread
	 * identity and state, stack info, and time info.
	 * Also, all auxiliary data structures are initialized:
	 * queue of threads this thread is waiting for (waitlist).
	 * After successful initialization, this PCB is added
	 * to a static list (queue) of all PCBs in memory.
	 */
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
	StackSize maxStackSize = 65536 / stackElementSize; // limit stack size to 64KB
	stackSize = stackSize > maxStackSize ? maxStackSize : stackSize;
	this->initializeStack(); // initialize this thread's stack
	waitList = new Deque<PCB*>(); // initialize waitList for this thread
	PCB::allPCB->push_back(this); // register this PCB in static list of all PCBs
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
}

PCB::~PCB() {
	/* Destructor does exactly the opposite, it removes this
	 * PCB from the static list (deque) of all PCBs, and more
	 * importantly - it deletes the dynamically allocated stack.
	 */
	PCB::allPCB->remove(this); // unregister this PCB
	delete waitList; // delete waitList
	waitList = NULL;
	this->deleteStack(); // delete stack
}

PCB* PCB::getPCBById(ID id) {
	/* Returns pointer to PCB with given id by
	 * searching a static list of all PCBs in
	 * the system. If PCB wasn't found, it
	 * returns NULL. This function is static.
	 */
	PCB::allPCB->toFront();
	while (PCB::allPCB->isThere()) {
		if (PCB::allPCB->getCurr()->id == id) {
			return PCB::allPCB->getCurr();
		}
		PCB::allPCB->toNext();
	}
	return NULL;
}

void PCB::wrapper() {
	/* Used as a wrapper function for calling running thread's
	 * polymorphic run() method (which is protected).
	 */
	// Call run (after this line, the thread is essentially finished)
	if (PCB::running->myThread) PCB::running->myThread->run();
	// All threads waiting for running thread should now get ready
	while(!PCB::running->waitList->isEmpty()) {
		// Get next thread from the deque
		PCB* tempPCB = PCB::running->waitList->front();
		PCB::running->waitList->pop_front();
		// Mark it ready and put it into the scheduler
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		tempPCB->state = READY;
		Scheduler::put(tempPCB);
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	}
	// After run() is finished, update running thread's state
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	PCB::running->state = FINISHED;
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
	// Finally, give over the CPU to another thread
	dispatch();
	// This line should absolutely never be reachead!
}
