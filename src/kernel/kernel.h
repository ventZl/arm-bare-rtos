#pragma once

#include "api.h"
#include "arm_arch.h"

#ifndef CPU_CORE_COUNT

#define CPU_CORE_COUNT  1
#define kernel_current_cpu() 0

#endif

#include <stdint.h>

/** Alias thread ID */
typedef uint8_t Thread_ID_t;

/** Structure holding current scheduling state of CPU.
 *
 * This structure holds complete state required to perform thread scheduling.
 * It is private to CPU. Kernel should allocate as many of these as there are
 * CPUs.
 */
struct OS_core_state_t {
	Thread_ID_t thread_prev;
	Thread_ID_t thread_current;
	Thread_ID_t thread_next;
};

extern struct OS_thread_t os_threads[];
