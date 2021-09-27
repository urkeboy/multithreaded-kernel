// File: pcb.h
// Uros Bojanic 2019/0077
#ifndef PCB_H_
#define PCB_H_

// Preprocessor directives
#include "thread.h"
#include "deque.h"
#include "lock.h"
#include "SCHEDULE.h"

// Thread state (finite set of 5 possible states)
enum State { NEW, READY, RUNNING, BLOCKED, FINISHED };

// PCB
class PCB {
public:
	// Static counter for ids
	static ID threadID;
	// This thread's id
	const ID id;

	// Stack size
	StackSize stackSize;
	// Stack
	unsigned* stack;
	// Stack segment
	unsigned ss;
	// Stack pointer
	unsigned sp;
	// Base pointer
	unsigned bp;
	// Initialize stack
	void initializeStack();
	// Delete stack
	void deleteStack();
	
	// Quant of CPU time associated to this thread
	Time timeSlice;

	// Thread associated with this PCB
	Thread* myThread;
	// Thread state of thread associated with this PCB
	volatile State state;
	// List (queue) of threads this thread is waiting for
	Deque<PCB*>* waitList;

	// Every PCB has an associated thread (1-1 relationship)
	friend class Thread;
	// Used to search all PCBs to find thread with specific id
	static PCB* getPCBById(ID id);

	// List of all PCBs in the system
	static Deque<PCB*>* allPCB;
	// Running thread pointer (static of course)
	static PCB* running;
	// Idle thread (does nothing, just loops forever)
	static PCB* idle;

	// Semaphore this thread is waiting on
	int semaphoreTimeLeft;

	// Used to call thread's polymorphic run() metheod
	static void wrapper();

	// Constructor - creates a new PCB with associated thread
	PCB(StackSize stackSize, Time timeSlice, Thread* myThread);
	// Destructor - deletes this PCB
	~PCB();
};

#endif /* PCB_H_ */
