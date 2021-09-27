// File: thread.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "pcb.h"
#include "lock.h"
#include "timer.h"
#include <stdio.h>

void Thread::start() {
	/* Starts this thread by setting its state to READY.
	 * and puts the thread into the scheduler. This is
	 * only done once (when the first call is made - i.e.
	 * thread state is NEW, and all subsequent calls to
	 * this method are guaranteed to have no effect).
	 */
	if (myPCB->state == NEW) {
		myPCB->state = READY; // mark thread as READY
		Scheduler::put(myPCB); // put it into the scheduler
	}
}

void Thread::waitToComplete() {
	/* Running thread should wait for this thread to
	 * complete first. This is only possible if this
	 * thread is either READY or BLOCKER (if this thread
	 * were: NEW - it hasn't been started yet, RUNNING -
	 * the thread can't wait for itself, FINISHED - there
	 * is nothing to wait for). We block the running
	 * thread and it to this thread's "waitlist" - list of
	 * threads that are waiting for this thread to finish.
	 * Also, in the end we need to call dispatch() and
	 * manually give over the CPU to another thread.
	 */
	if ((myPCB->state == READY || myPCB->state == BLOCKED) && myPCB != PCB::idle) {
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		PCB::running->state = BLOCKED; // block running thread
		myPCB->waitList->push_back(PCB::running); // add it to the waitlist
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
		dispatch();
	}
}

Thread::~Thread() {
	/* Thread doesn't use any dynamic memory except for
	 * allocation of associated PCB. We delete it, but
	 * first we need to ensure the running thread waits
	 * for this thread to finish its execution. This is
	 * important, so as to avoid premature deleting the
	 * Thread object before run() method has finished.
	 */
	this->waitToComplete(); // wait for this thread to finish
	delete myPCB; myPCB = NULL; // delete this thread's PCB
}

ID Thread::getId() {
	/* Returns this thread's unique identifier (ID)
	 * from its associated PCB (const int field).
	 */
	return myPCB->id;
}

ID Thread::getRunningId() {
	/* Returns running thread's unique identifier (ID)
	 * from its associated PCB (const int field).
	 */
	return PCB::running->id;
}

Thread* Thread::getThreadById(ID id) {
	/* Returns pointer to thread with given id by
	 * calling a static PCB function that returns
	 * PCB of the thread with this particular id.
	 * If no thread with this id was found, it
	 * returns NULL. This function is static.
	 */
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	PCB* pcb = PCB::getPCBById(id);
	PCB::allPCB->toNull();
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
	if (pcb) return pcb->myThread;
	return NULL;
}

Thread::Thread(StackSize stackSize, Time timeSlice) {
	/* stackSize ranges from 0 to 32767 (max. 65535B for 8086)
	 * timeSlice ranges from 0 to inf   (0 = infinite assumed)
	 * Only delegates the call to associated PCB
	 * constructor, as Thread doesn't store any fields
	 * itself - all attributes are stored in PCB.
	 */
	this->myPCB = new PCB(stackSize, timeSlice, this);
}

void dispatch() {
	/* Dispatch provides thread with the ability to
	 * give over the CPU to another thread pulled
	 * from the scheduler, on explicit request.
	 * This is implemented using a timer.
	 */
#ifndef BCC_BLOCK_IGNORE
	lock
#endif
	context_switch_on_demand = 1; // set timer flag
	Timer::timer(); // call timer ( <=> asm int 8h;)
#ifndef BCC_BLOCK_IGNORE
	unlock
#endif
}
