// File: kernel.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "kernel.h"
#include "timer.h"
#include "lock.h"

// System interrupt routines (old & new)
pInterrupt Kernel::oldISR = NULL;
pInterrupt Kernel::newISR = Timer::timer;

// Old interrupt routine - seg & offset (deprecated)
// unsigned oldTimerOFF, oldTimerSEG; // use <dos.h> for SEG,OFF

void Kernel::inic() {
	/* Initializes the system by moving the old
	 * interrupt routine (oldISR) to entry 60h and
	 * loading the new one (timer) to entry 08h.
	 * This method is locked because it interferes
	 * directly with interrupt vector table (IVT).
	 */
#ifndef BCC_BLOCK_IGNORE
	lock
	Kernel::oldISR = getvect(0x08); // save old interrupt routine
	setvect(0x60, Kernel::oldISR); // move old interrupt routine
	setvect(0x08, Kernel::newISR); // replace it with the new one
	unlock
#endif
}
// NOTICE - Reference: "v2_zad3.cpp".

void Kernel::restore() {
	/* Restores the original system state by
	 * returning the old interrupt routine (oldISR)
	 * to its original entry (08h) in IVT.
	 * This method is locked because it interferes
	 * directly with interrupt vector table (IVT).
	 */
#ifndef BCC_BLOCK_IGNORE
	lock
	setvect(0x08, Kernel::oldISR); // restore old interrupt routine
	unlock
#endif
}
// NOTICE - Reference: "v2_zad3.cpp".
