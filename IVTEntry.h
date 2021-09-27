// File: IVTEntry.h
// Uros Bojanic 2019/0077
#ifndef IVTENTRY_H_
#define IVTENTRY_H_

// Preprocessor directives
#include "kernel.h"
#include "lock.h"

// Interrupt Vector Table Entry number
typedef unsigned char IVTNo;
// Interrupt routine pointer type (pointer to a function)
typedef void interrupt (*pInterrupt)(...);

// Event
class KernelEv;

// Interrupt Vector Table Entry
class IVTEntry {
public:
	// Constructor - creates new IVT entry
	IVTEntry(IVTNo ivtNo, pInterrupt newISR);
	// Destructor - deletes this IVT entry
	virtual ~IVTEntry();
	// Array of all IVT entries
	static IVTEntry* allIVTE[256];
	// Event associated with this IVT entry
	KernelEv* myEvent;
	// Signal event
	void signal();
	// Call old interrupt routine
	void oldISR_invoke();
protected:
	// New interrupt routine in this IVT entry
	pInterrupt newISR;
	// Old interrupt routine in this IVT entry
	pInterrupt oldISR;
private:
	// Interrupt Vector Table Entry number
	IVTNo ivtNo;
};

#endif /* IVTENTRY_H_ */
