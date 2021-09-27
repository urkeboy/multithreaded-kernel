// File: lock.cpp
// Uros Bojanic 2019/0077

// Preprocessor directives
#include "lock.h"

// Flag for explicit context switch lock (nested locking ensured)
volatile int prevent_context_switching = 0;
