// File: kernel.h
// Uros Bojanic 2019/0077

#ifndef KERNEL_H_
#define KERNEL_H_

// Preprocessor directives
#include <iostream.h>
#include <fstream.h>
#include <dos.h>

// Interrupt routine pointer type (pointer to a function)
typedef void interrupt (*pInterrupt)(...);

// System Settings
class Kernel{
private:
	// Old interrupt routine from IVT:0x08
	static pInterrupt oldISR;
	// New interrupt routine into IVT:0x08
	static pInterrupt newISR;
public:
	// Initialize system settings
	static void inic();
	// Restore system settings
	static void restore();
};

#endif /* KERNEL_H_ */
