// File: semaphor.h
// Uros Bojanic 2019/0077
#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

// Preprocessor directives
#include "thread.h"

// Kernel implementation of the semaphore
class KernelSem;

// Semaphore
class Semaphore {
public:
	// Constructor - makes a semaphore
	Semaphore(int init = 1);
	// Destructor - deletes a semaphore
	virtual ~Semaphore();

	// Wait operation
	virtual int wait(Time maxTimeToWait);
	// Signal operation
	virtual void signal();

	// Returns the current value of the semaphore
	int val() const;
private:
	// Kernel implementation of semaphore (1-1 association)
	KernelSem* myImpl;
};

#endif /* SEMAPHOR_H_ */
