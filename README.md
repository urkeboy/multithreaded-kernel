# multithreaded-kernel
[Operating Systems] Multithreaded kernel implementation in C++ with time sharing, semaphores, and events.

Homework/project in **Operating Systems 1 (13E112OS1)** at the University of Belgrade, School of Electrical Engineering.

## Kernel

This project provides a small but fully functional operating system kernel that supports threads (multithreaded OS). This kernel provides the concept of threads, the services of creating and running threads, and the concept of semaphores, events, and time-sharing.

### Logical Structure

```
📦kernel
 ┃
 ┣ <a href="#threads">threads</a> [`threads`](#threads)
 ┃ ┣ wrapper
 ┃   ┣ 📜thread.h
 ┃   ┗ 📜thread.cpp
 ┃ ┣ implementation
 ┃   ┣ 📜pcb.h
 ┃   ┗ 📜pcb.cpp
 ┃ ┗ special
 ┃   ┣ 📜idle.h
 ┃   ┗ 📜idle.cpp
 ┃
 ┣ [`semaphores`](#semaphores)
 ┃ ┣ wrapper
 ┃   ┣ 📜semaphor.h
 ┃   ┗ 📜semaphor.cpp
 ┃ ┗ implementation
 ┃   ┣ 📜kerSem.h
 ┃   ┗ 📜kerSem.cpp
 ┃
 ┣ [`events`](#events)
 ┃ ┣ wrapper
 ┃   ┣ 📜event.h
 ┃   ┗ 📜event.cpp
 ┃ ┣ implementation
 ┃   ┣ 📜kerEv.h
 ┃   ┗ 📜kerEv.cpp
 ┃ ┗ auxiliary
 ┃   ┣ 📜IVTEntry.h
 ┃   ┗ 📜IVTEntry.cpp
 ┃
 ┣ [`preemption`](#preemption)
 ┃ ┣ locks
 ┃   ┣ 📜lock.h
 ┃   ┗ 📜lock.cpp
 ┃ ┣ timer
 ┃   ┣ 📜timer.h
 ┃   ┗ 📜timer.cpp
 ┃ ┗ setup
 ┃   ┣ 📜kernel.h
 ┃   ┗ 📜kernel.cpp
 ┃
 ┗ subsystem
   ┗ 📜main.cpp
```

### Threads

This kernel provides a thread subsystem - including the concept of threads in a multithreaded operating system, and the following operations:
- `Thread::Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice)` create a new thread in the kernel with a fixed designated timeSlice (for timeSlice = 0, the created thread has an unlimited execution time interval)
- `void Thread::start()` start a new execution flow for created thread
- `void dispatch()` explicit dispatch
- `ID Thread::getId()` unique thread identification number
- `static Thread* getThreadById(ID id)` thread retrieval based on the identification number
- `static ID getRunningId()` running thread's identification number retrieval

The solution provides creating an unlimited number of user threads (bounded only by available memory with a control stack of at least 64KB in size.

Class `Thread` serves as a wrapper for kernel's implementation of thread - class `PCB`. Each object of class `Thread` has a 1-1 connection with a corresponding instance of class `PCB`.

A special thread that exists in this thread subsystem is the `Idle` thread, which is scheduled only when there are no other ready threads.

### Semaphores

This kernel provides the concept of a standard counting semaphores with wait and signal operations, and a time-limited waiting interval. If the semaphore value is negative (`val < 0`), then there are `|val|` blocked threads on the semaphore.

The `int Semaphore::wait()` accepts an integer indicating the maximum duration of the blocking of the calling thread (in time-slices, or 0 for unlimited time) and returns 1 if a thread is unblocked by calling the `void Semaphore::signal()`operation.

Class `Semaphore` serves as a wrapper for kernel's implementation of semaphore - class `KernelSem`. Each object of class `Semaphore` has a 1-1 connection with a corresponding instance of class `KernelSem`.

### Events

This kernel provides the concept of an event - a binary semaphore. This allows for sporadic processes. If the interrupt occurs periodically, then the same principle can be used for the realization of periodic processes.

Class `Event` serves as a wrapper for kernel's implementation of semaphore - class `KernelEv`. Each object of class `Event` has a 1-1 connection with a corresponding instance of class `KernelEv`.

### Preemption

This kernel provides context switching and preemption in the following cases:
- at an explicit request of a user thread (synchronous), by calling the `dispatch()` function
- on the occurrence of interrupts (asynchronous), when events are used
- due to an operation on a synchronization primitive (synchronous), either semaphores or events
- after the time slot assigned to the process expires (time-sharing)
- at an explicit request to "join" a thread (wait for its completion), by calling the `waitToComplete()` function
