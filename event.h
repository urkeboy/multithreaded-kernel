// File: event.h
// Uros Bojanic 2019/0077

#ifndef EVENT_H_
#define EVENT_H_

// Preprocessor directives
#include "IVTEntry.h"

#define PREPAREENTRY(ivtNo, callOld) \
void interrupt newISR##ivtNo(...); \
IVTEntry entry##ivtNo(ivtNo, newISR##ivtNo); \
void interrupt newISR##ivtNo(...) { \
	if (callOld) { \
		entry##ivtNo.oldISR_invoke(); \
	} \
	entry##ivtNo.signal(); \
	dispatch(); \
} \

// Interrupt Vector Table Entry number
typedef unsigned char IVTNo;

// Kernel implementation of the event
class KernelEv;

// Event
class Event {
public:
	// Constructor - makes an event
	Event(IVTNo ivtNo);
	// Destructor - deletes an event
	~Event();
	// Wait operation (can only be called by owner thread)
	void wait();
protected:
	friend class KernelEv;
	// Signal operation (can call KernelEv)
	void signal();
private:
	// Kernel implementation of event (1-1 association)
	KernelEv* myImpl;
};

#endif /* EVENT_H_ */
