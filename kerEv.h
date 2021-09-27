// File: kerEv.h
// Uros Bojanic 2019/0077
#ifndef KEREV_H_
#define KEREV_H_

// Preprocessor directives
#include "event.h"
#include "pcb.h"
#include "lock.h"

// Event
class Event;
class IVTEntry;

// Kernel implementation of event
class KernelEv {
public:
	// Constructor - makes an event
	KernelEv(IVTNo ivtNo);
	// Destructor - deletes an event
	~KernelEv();

	// Wait operation
	void wait();
	// Signal operation
	void signal();
protected:
	// Owner thread
	PCB* owner;
private:
	// Current value of the event
	int value;
	// Interrupt Vector Table Entry number
	int ivtNo;
	// Blocked thread
	PCB* blockedThread;
};

#endif /* KEREV_H_ */
