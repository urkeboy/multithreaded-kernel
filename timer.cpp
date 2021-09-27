// File: timer.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "timer.h"
#include "lock.h"
#include "pcb.h"
#include "kerSem.h"
#include <stdio.h>

// Temporary variables for context switching
volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;

// Timer-related variables - counter and flags
volatile int Timer::cnt = 20;
volatile int context_switch_on_demand = 0;
volatile int context_switch_on_retrial = 0;

// Implemented in the test environment
extern void tick();

void interrupt Timer::timer(...) {
	/* Provides context switching in the event of explicit
	 * demand, or when the time given to this thread
	 * has expired (counter fell from timeSlice to 0).
	 * This timer is the only component in our kernel that
	 * provides context switching, and is therefore responsible
	 * for time sharing. This kernel is set up to use hardware
	 * interrupts on IVT entry 0x08 (18.2 times per second, or
	 * equivalent of 55ms ticks).
	 */
	// Tick all semaphores once every 55ms (decreases thread sleep timers)
	if (!context_switch_on_demand) {
		KernelSem::tickAllSEM();
	}
	if (!context_switch_on_demand && Timer::cnt > 0) Timer::cnt--;
	if (context_switch_on_demand || Timer::cnt == 0 || context_switch_on_retrial) {
		if (prevent_context_switching > 0) {
			// If context switch was prevented explicitly, try again later
			context_switch_on_retrial = 1;
		}
		else {
			// Save context
			#ifndef BCC_BLOCK_IGNORE
			asm{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}
			#endif
			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;
			// Put old thread into the scheduler (unless it's Idle)
			if(PCB::running->state != BLOCKED && PCB::running->state != FINISHED && PCB::running != PCB::idle) { // && PCB::running->state == RUNNING ??
				PCB::running->state = READY;
				Scheduler::put(PCB::running);
			}
			// Get new thread from the scheduler
			PCB::running = Scheduler::get();
			// If no threads are in the scheduler, run Idle
			PCB::running = PCB::running ? PCB::running : PCB::idle;
			PCB::running->state = RUNNING;
			// If timeSlice=0, presume infinite cnt (execute without interruption)
			Timer::cnt = PCB::running->timeSlice ? PCB::running->timeSlice : -1;
			// Restore context
			tsp = PCB::running->sp;
			tss = PCB::running->ss;
			tbp = PCB::running->bp;
			#ifndef BCC_BLOCK_IGNORE
			asm{
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
			#endif
			// No second try needed, context switch was successful
			context_switch_on_retrial = 0;
		}
	}
	// Notify the old 0x08 interrupt routine if needed (moved to 0x60)
	if (!context_switch_on_demand) {
		tick();
		#ifndef BCC_BLOCK_IGNORE
		asm int 60h;
		#endif
	}
	// Explicit demand is guaranteed to be successfully serviced
	else {
		context_switch_on_demand = 0;
	}
}
// NOTICE - Reference: "v2_zad4.cpp".
