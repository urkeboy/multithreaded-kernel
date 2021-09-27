// File: idle.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "idle.h"
#include <stdio.h>

Idle::Idle() : Thread(defaultStackSize, 1) {
	/* Initializes a thread that idles -
	 * i.e. does nothing (loops forever).
	 */
}

void Idle::run() {
	/* Do nothing.
	 */
	for (int j = 0; j < 32000; j++) {
		if (j == 30000) j = 0;
	}
}

// Destructor is called implicitly.
// Idle::~Idle() { }
