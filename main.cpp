// File: main.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "pcb.h"
#include "idle.h"
#include "kernel.h"
#include "lock.h"
#include "timer.h"
#include "semaphor.h"
#include "event.h"
#include "SCHEDULE.h"
#include <stdio.h>
#include <dos.h>
#include <stdarg.h>

// User main thread code
int userMain(int argc, char** argv);

// System threads
Idle* idleThread; // id=1
PCB*  mainThread; // id=2

// Main
int main(int argc, char** argv) {
	/* Initializes system by replacing interrupt
	 * routine in IVT at entry 0x08, calling the
	 * user main function, and finally restoring
	 * the original interrupt routine in the end.
	 */
	// Start
	cout << "Main starts.\n\n"; // DEBUG
	// Set new interrupt routine (entry 08h in Interrupt Vector Table)
	Kernel::inic();
	// Make system threads (Idle, Main) - stored as static fields in PCB class
	idleThread  = new Idle();
	PCB::idle = idleThread->myPCB;
	PCB::idle->state == READY;
	mainThread = new PCB(4096, 1, NULL);
	PCB::running = mainThread;
	PCB::running->state == RUNNING;
	// Going to userMain in order to execute user code
	cout << "jumping to userMain...\n\n"; // DEBUG
	// Call userMain function and remember its return value for possible errors
	int status = userMain(argc, argv);
	cout << "\n... returning to main (status=" << status << ")\n"; // DEBUG
	// Restore old interrupt routine (entry 08h in Interrupt Vector Table)
	Kernel::restore();
	// End
	cout << "\nMain ends.\n"; // DEBUG
	return 0;
}
