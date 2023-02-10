#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

/** Error states returned by the kernel
 */
enum OS_Error {
    OS_E_OK = 0,            // No error
    OS_E_NULLPTR = 1,       // NULL pointer provided
    OS_E_BUSY = 2,          // Thread already exists, cannot be created again
};

typedef uint8_t Thread_ID_t;

/** List of states in which thread can be.
 */
enum OS_ThreadState {
	/// This thread slot is empty (default after reset)
	THREAD_STATE_EMPTY = 0,
	/// Thread is ready to be scheduled
	THREAD_STATE_READY,
	/// Thread is currently running
	THREAD_STATE_RUNNING,
};

/** Thread control block.
 *
 * This structure holds current status of the thread.
 */
struct OS_Thread_t {
	/** Value of SP. This is only valid if thread is in state different than
	 * THREAD_STATE_RUNNING. Obviously it is undefined for empty thread slots
	 * and slots which don't have stack allocated yet.
	 */
	uint32_t * sp;

	/** State of this thread. */
	enum OS_ThreadState state;
};

/** Opt for thread switch on current core.
 * Calling this function will prepare task switch. It will set up
 * some stuff needed and then schedule PendSV. You have to call this
 * function from supervisor mode. You can enter it either via timer
 * ISR, or implement it as SVC, which will yield.
 * @param next_thread address of thread struct for thread, which should be scheduled next
 */
bool os_schedule_context_switch(struct OS_Thread_t * next_thread);

/** Start operating system on current core.
 * This will leave supervisor mode into thread mode, switching into
 * thread-private stack. This function will automatically detect,
 * which core it is running on. You can call it multiple times on 
 * multiple cores.
 * @param startup_thread address of thread struct which shall be ran
 */
void __attribute__((noreturn)) os_start(struct OS_Thread_t * startup_thread);

/** Get thread ID of currently running thread.
 * @returns the address structure describing currently running thread 
 */
struct OS_Thread_t * os_get_current_thread();

/** Create new thread.
 * Will populate thread table with new entry. Thread just created
 * is put into ready state, which means it can be immediately executed.
 * @param thread address of thread structure, which has to be initialized
 * @param entrypoint address of the "main" function for this thread
 * @param data address of the data, which will be passed to the entrypoint function
 * @param stack base address of thread's stack
 * @param stack_size size of thread's stack
 * @returns @ref OS_Error status of the operation. OS_E_OK if thread was created,
 * OS_E_NULLPTR if thread pointer is a NULL pointer, OS_E_BUSY if thread is already 
 * created.
 */
int os_thread_create(struct OS_Thread_t * thread, void (* entrypoint)(void*), void * data, uint8_t * stack, uint32_t stack_size);

#ifdef __cplusplus
} // extern "C"
#endif
