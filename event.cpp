// File: event.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "event.h"
#include "kerEv.h"
#include "lock.h"
#include <stdlib.h>

Event::Event(IVTNo ivtNo) {
	/* Creates new Event object by delegating the
	 * call to KernelEv constructor (kernel implementation
	 * of Event is stored as a dynamic variable).
	 * This function is as a wrapper with locking ensured.
	 */
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	myImpl = new KernelEv(ivtNo); // create event
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
}

Event::~Event() {
	/* Deletes this Event object by destructing the
	 * associated KernelEv object (kernel implementation
	 * of Event is stored as a dynamic variable).
	 * This function is as a wrapper with locking ensured.
	 */
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	delete myImpl; // delete event
	myImpl = NULL;
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
}

void Event::wait() {
	/* Delegates call to KernelEv's wait() implementation.
	 * This function is as a wrapper with locking ensured later.
	 */
	myImpl->wait();
}

void Event::signal() {
	/* Delegates call to KernelEv's signal() implementation.
	 * This function is as a wrapper with locking ensured later.
	 */
	myImpl->signal();
}
