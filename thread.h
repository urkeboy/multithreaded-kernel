// File: thread.h
// Uros Bojanic 2019/0077
#ifndef _thread_h_
#define _thread_h_

// Preprocessor directives
#include <stdlib.h>

// Stack info
typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

// Time info
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms

// Thread identifier
typedef int ID;

// Kernel's implementation of a user's thread
class PCB;

// Thread
class Thread {
public:
	// Starts the thread and puts it into the scheduler
	void start();
	// Running thread is set to wait for this thread to complete
	void waitToComplete();
	// Destructor - virtual because classes will be derived from Thread
	virtual ~Thread();

	// Returns this thread's unique identifier (int)
	ID getId();
	// Returns current running thread's unique identifier
	static ID getRunningId();
	// Returns thread with given unique identifier
	static Thread* getThreadById(ID id);

	// Pointer to this thread's associated PCB
	PCB* myPCB;

protected:
	// Every thread has an associated PCB (1-1 relationship)
	friend class PCB;

	// Constructor - creates a new thread and its associated PCB
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);

	// Polymorphic method (should be implemented in classes derived from Thread)
	virtual void run() {}

private:

};

// Dispatch - explicit assumption
void dispatch();

#endif
