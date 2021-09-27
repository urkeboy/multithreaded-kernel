// File: kerEv.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "kerEv.h"
#include "IVTEntry.h"
#include "lock.h"
#include "pcb.h"
#include <stdio.h>

KernelEv::KernelEv(IVTNo ivtNo) : owner(PCB::running), ivtNo(ivtNo), value(0), blockedThread(NULL) {
	/* Creates new KernelEv object and initializes its fields.
	 * Also, appropriate IVT Entry is initialized as well.
	 */
	IVTEntry::allIVTE[ivtNo]->myEvent = this; // associate this event with its IVT entry
}

KernelEv::~KernelEv() {
	/* Deletes this KernelEv object and frees the appropriate IVT Entry.
	 * It is important to first check if the entry itself wasn't deleted.
	 */
	if (IVTEntry::allIVTE[ivtNo])
		if (IVTEntry::allIVTE[ivtNo]->myEvent == this)
			IVTEntry::allIVTE[ivtNo]->myEvent = NULL; // remove this event from IVT entry
}

void KernelEv::wait() {
	/* Wait operation on the event decreases its value
	 * and checks if running thread should be blocked.
	 * Called by owner thread only.
	 */
	if (PCB::running != owner) return; // calling thread isn't the owner
	if (value == 1) {
		value = 0;
		return; // owner already blocked on this event
	}
	else {
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	owner->state = BLOCKED; // block owner thread
	blockedThread = PCB::running;
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
	dispatch();
	}
}

void KernelEv::signal() {
	/* Signal operation on the event increases its value
	 * and checks if owner thread should be unblocked.
	 * Called by interrupt routine only.
	 */
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_lock(); // lock
#endif
	value = blockedThread ? 0 : 1; // 1 if thread was not blocked to begin with
	if(blockedThread) {
		blockedThread->state = READY; // unblock thread
		Scheduler::put(blockedThread);
		blockedThread = NULL;
	}
#ifndef BCC_BLOCK_IGNORE
	prevent_context_switching_unlock(); // unlock
#endif
}
