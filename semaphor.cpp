// File: semaphor.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "semaphor.h"
#include "kerSem.h"
#include "lock.h"
#include <stdlib.h>

Semaphore::Semaphore(int init) {
	/* Creates new Semaphore object by delegating the
	 * call to KernelSem constructor (kernel implementation
	 * of Semaphore is stored as a dynamic variable).
	 * This function is as a wrapper with locking ensured.
	 */
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	myImpl = new KernelSem(init); // create semaphore
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
}

Semaphore::~Semaphore() {
	/* Deletes this Semaphore object by destructing the
	 * associated KernelSem object (kernel implementation
	 * of Semaphore is stored as a dynamic variable).
	 * This function is as a wrapper with locking ensured.
	 */
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	delete myImpl; // delete semaphore
	myImpl = NULL;
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
}

int Semaphore::wait(Time maxTimeToWait) {
	/* Delegates call to KernelSem's wait() implementation.
	 * This function is as a wrapper with locking ensured later.
	 */
	return myImpl->wait(maxTimeToWait);
}

void Semaphore::signal() {
	/* Delegates call to KernelSem's signal() implementation.
	 * This function is as a wrapper with locking ensured later.
	 */
	myImpl->signal();
}

int Semaphore::val() const {
	/* Delegates call to KernelSem's val() implementation.
	 */
	return myImpl->val();
}
