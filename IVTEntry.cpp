// File: IVTEntry.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "IVTEntry.h"
#include "kerEv.h"
#include <dos.h>

// Initialize static variables
IVTEntry* IVTEntry::allIVTE[256] = {NULL};

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newISR) : ivtNo(ivtNo), oldISR(NULL), newISR(newISR), myEvent(NULL) {
	/* Creates IVT Entry object, and initializes it fields.
	 * Old interrupt routine in entry #ivtNo is backed up
	 * and replaced by the new one. For ease of further use,
	 * the static array of all entries is updated accordingly.
	 * This method is strictly locked because it interferes
	 * directly with interrupt vector table (IVT).
	 */
#ifndef BCC_BLOCK_IGNORE
	lock
	oldISR = getvect(ivtNo); // save old interrupt routine
	setvect(ivtNo, newISR); // replace it with the new one
	IVTEntry::allIVTE[ivtNo] = this; // save entry in static array of all entries
	unlock
#endif
}

IVTEntry::~IVTEntry() {
	/* Deletes this IVT Entry object (has no dynamic memory).
	 * Old interrupt routine in returned to its original entry,
	 * and removed from the static array of all entries.
	 * This method is strictly locked because it interferes
	 * directly with interrupt vector table (IVT).
	 */
	oldISR_invoke();
#ifndef BCC_BLOCK_IGNORE
	lock
	setvect(ivtNo, oldISR); // restore old interrupt routine
	// oldISR = NULL;
	// newISR = NULL;
	// IVTEntry::allIVTE[ivtNo] = NULL; // remove entry from static array of all entries
	unlock
#endif
}

void IVTEntry::signal() {
	/* Signals the event associated to this IVT entry.
	 */
	if (!myEvent) return;
	myEvent->signal();
}


void IVTEntry::oldISR_invoke() {
	/* Invokes old interrupt routine on this IVT entry.
	 * Locking is not needed at this point.
	 */
	if (!oldISR) return;
	(*oldISR)(); // call old interrupt routine
}
