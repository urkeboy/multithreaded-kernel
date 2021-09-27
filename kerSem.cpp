// File: kerSem.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "kerSem.h"
#include "lock.h"
#include "pcb.h"
#include <stdio.h>

// Initializing static variables
Deque<KernelSem*>* KernelSem::allSEM = new Deque<KernelSem*>();

KernelSem::KernelSem(int init) : value(init) {
	/* Creates new KernelSem object and adds it to allSEM list.
	 * Initializes list of threads waiting on this semaphore.
	 */
	allSEM->push_back(this); // add semaphore to the static list
	blockedThreads = new Deque<PCB*>(); // initialize blocked list
}

KernelSem::~KernelSem() {
	/* Deletes this KernelSem object and removes it from allSEM list.
	 * Deletes the list of threads waiting on this semaphore.
	 */
	allSEM->remove(this); // remove semaphore from the static list
	delete blockedThreads; // delete blocked list
	blockedThreads = NULL;
}

int KernelSem::wait(Time maxTimeToWait) {
	/* Wait operation on the semaphore decreases its
	 * value and checks if running thread needs to be blocked.
	 */
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	value -= 1; // decrease semaphore value
	if (value < 0) {
		block(maxTimeToWait); // block running thread (add it to the blocked list)
#ifndef BCC_BLOCK_IGNORE
		unlock
#endif
		dispatch(); // explicit context switch is needed to continue the program
		return PCB::running->semaphoreTimeLeft != -1; // 0 if timeout, 1 if signal called
	}
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif
	return 1; // 1 if no blocking occured
}

void KernelSem::signal() {
	/* Signal operation on the semaphore increases its
	 * value and checks if a thread can be unblocked.
	 */
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	value += 1; // increase semaphore value
	if (value < 1) {
		deblock(NULL); // deblock first thread from the blocked list
	}
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif
}

int KernelSem::val() const {
	/* Returns the current value of the semaphore (int).
	 * If val < 0, then -val threads are waiting on it.
	 */
	return value;
}

void KernelSem::block(int maxTimeToWait) {
	/* Blocks the running thread and adds it to the blocked list.
	 * This method provides locking (critical section: blockedThreads).
	 */

	PCB::running->semaphoreTimeLeft = (maxTimeToWait > 0) ? maxTimeToWait : -1; // set waiting timeout (-1 if wait(0) called)
	PCB::running->state = BLOCKED; // block running thread
	blockedThreads->push_back(PCB::running); // add thread to the blocked list

}

void KernelSem::deblock(PCB* pcb) {
	/* Deblocks one thread and removes it from the blocked list.
	 * This method provides locking (critical section: blockedThreads).
	 */
	if (blockedThreads->isEmpty()) {
		printf("CRITICAL ERROR! (kerSem.cpp).\n"); // blocked list empty
		return;
	}
	PCB* tmpPCB = pcb ? pcb : blockedThreads->front(); // if NULL is passed, get the first element
	blockedThreads->remove(tmpPCB); // update blocked list
	tmpPCB->state = READY; // make deblocked thread ready
	Scheduler::put(tmpPCB); // put it into the scheduler
}

void KernelSem::tickAllSEM() {
	/* Ticks all semaphores once (called from timer every 55ms),
	 * by calling tickSemaphore() for each semaphore (static method).
	 */
	KernelSem::allSEM->toFront(); // iterate over all semaphores
	while (KernelSem::allSEM->isThere()) {
		KernelSem::allSEM->getCurr()->tickSemaphore(); // tick semaphore
		KernelSem::allSEM->toNext();
	}
	KernelSem::allSEM->toNull();
}

void KernelSem::tickSemaphore() {
	/* Ticks this semaphores once, meaning all semaphoreTimeLeft counters
	 * of threads waiting on this semaphore are decreased by 1, and checked
	 * for possible timeout and early deblocking (without signal).
	 */
	Deque<PCB*>* toBeUnblocked = new Deque<PCB*>();
	// iterate over all blocked threads
	blockedThreads->toFront();
	while (blockedThreads->isThere()) {
		PCB* tmpPCB = blockedThreads->getCurr();
		if (--(tmpPCB->semaphoreTimeLeft) == 0) { // threads that timed out
			toBeUnblocked->push_back(tmpPCB);
		}
		if (tmpPCB->semaphoreTimeLeft <= 0) { // threads without timeout
			tmpPCB->semaphoreTimeLeft = -1;
		}
		tmpPCB = NULL;
		blockedThreads->toNext();
	}
	blockedThreads->toNull();
	// deblock all threads that timed out
	while(!(toBeUnblocked->isEmpty())) {
		PCB* tmpPCB = toBeUnblocked->front();
		deblock(tmpPCB); // deblock thread
		value += 1; // update semaphore value (critical to be done explicitly here)
		tmpPCB = NULL;
		toBeUnblocked->pop_front();
	}
	toBeUnblocked->toNull();
	delete toBeUnblocked;
	toBeUnblocked = NULL;
}
