// File: idle.h
// Uros Bojanic 2019/0077
#ifndef IDLE_H_
#define IDLE_H_

// Preprocessor directives
#include "thread.h"

// Idle
class Idle : public Thread{
public:
	Idle();
	virtual void run();
};

#endif /* IDLE_H_ */
