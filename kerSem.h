// File: kerSem.h
// Uros Bojanic 2019/0077
#ifndef KERSEM_H_
#define KERSEM_H_

// Preprocessor directives
#include "deque.h"
#include "semaphor.h"

// Semaphore
class Semaphore;

// Kernel implementation of semaphore
class KernelSem {
public:
	// Constructor - makes a semaphore
	KernelSem(int init);
	// Destructor - deletes a semaphore
	~KernelSem();

	// Wait operation
	int wait(Time maxTimeToWait);
	// Signal operation
	void signal();

	// Returns the current value of the semaphore
	int val() const;

	// List of all semaphores in the system
	static Deque<KernelSem*>* allSEM;
	// Tick all semaphores once (called from timer)
	static void tickAllSEM();
	// Tick this semaphore once
	void tickSemaphore();
protected:
	// List of threads blocked on this semaphore
	Deque<PCB*>* blockedThreads;
	// Blocks the running thread on this semaphore
	void block(int maxTimeToWait);
	// Unblocks one thread waiting on this semaphore
	void deblock(PCB* pcb = NULL);
private:
	// Current value of the semaphore
	int value;
};

#endif /* KERSEM_H_ */
