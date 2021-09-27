// File: timer.h
// Uros Bojanic 2019/0077

#ifndef TIMER_H_
#define TIMER_H_

// Preprocessor directives
#include "SCHEDULE.H"

// Flag for explicit dispatch() call
extern volatile int context_switch_on_demand;

// Timer
class Timer {
private:
	// Counter
	static volatile int cnt;
public:
	// Timer interrupt routine
	static void interrupt timer(...);
};

#endif /* TIMER_H_ */
