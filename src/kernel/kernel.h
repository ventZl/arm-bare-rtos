#pragma once

#include "api.h"
#include "arm_arch.h"

#ifndef CPU_CORE_COUNT

#define CPU_CORE_COUNT  1
#define kernel_current_cpu() 0

#endif

#include <stdint.h>

/* The current thread stack frame / saved context on the stack. */
struct Frame {
    uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
    uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
};

/** Structure holding current scheduling state of CPU.
 *
 * This structure holds complete state required to perform thread scheduling.
 * It is private to CPU. Kernel should allocate as many of these as there are
 * CPUs.
 */
struct OS_core_state_t {
	struct OS_Thread_t * thread_prev;
	struct OS_Thread_t * thread_current;
	struct OS_Thread_t * thread_next;
};
