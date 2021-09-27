// File: lock.h
// Uros Bojanic 2019/0077
#ifndef LOCK_H_
#define LOCK_H_

// Flag for explicit context switch lock (nested locking ensured)
extern volatile int prevent_context_switching;

// Lock critical section using previous flag
static inline void prevent_context_switching_lock() {
	/* Increases lock counter by 1, without any condition.
	 */
	prevent_context_switching += 1;
}

// Unlock critical section using previous flag
static inline void prevent_context_switching_unlock() {
	/* Decreases lock counter by 1, but never below 0.
	 */
	prevent_context_switching -= prevent_context_switching > 0 ? 1 : 0;
}

// lock - disables hardware interrupts (nested locking ensured)
#ifndef BCC_BLOCK_IGNORE
#define lock asm{\
 		pushf;\
 		cli;\
}
#endif
// NOTICE - Reference: "v2_zad3.cpp".

// unlock - enables hardware interrupts (nested locking ensured)
#ifndef BCC_BLOCK_IGNORE
#define unlock asm popf
#endif
// NOTICE - Reference: "v2_zad3.cpp".

#endif /* LOCK_H_ */
