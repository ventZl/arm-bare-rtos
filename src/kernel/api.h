#pragma once

#include <stdint.h>
#include <stdbool.h>

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
struct OS_thread_t {
	/** Value of SP. This is only valid if thread is in state different than
	 * THREAD_STATE_RUNNING. Obviously it is undefined for empty thread slots
	 * and slots which don't have stack allocated yet.
	 */
	uint32_t * sp;

	/** State of this thread. */
	enum OS_ThreadState state;
};

#define THREAD_TABLE_FULL 0xFF

/** Opt for thread switch on current core.
 * Calling this function will prepare task switch. It will set up
 * some stuff needed and then schedule PendSV. You have to call this
 * function from supervisor mode. You can enter it either via timer
 * ISR, or implement it as SVC, which will yield.
 * @param next_thread_id ID of thread, which should be scheduled next
 */
bool os_schedule_context_switch(Thread_ID_t next_thread_id);

/** Start operating system on current core.
 * This will leave supervisor mode into thread mode, switching into
 * thread-private stack. This function will automatically detect,
 * which core it is running on. You can call it multiple times on 
 * multiple cores.
 * @param startup_thread ID of thread which shall be ran
 */
void __attribute__((noreturn)) os_start(Thread_ID_t startup_thread);

/** Get thread ID of currently running thread.
 * @returns ID of currently running thread
 */
Thread_ID_t os_get_current_thread();

/** Create new thread.
 * Will populate thread table with new entry. Thread just created
 * is put into ready state, which means it can be immediately executed.
 * @param entrypoint address of the "main" function for this thread
 * @param data address of the data, which will be passed to the entrypoint function
 * @param stack base address of thread's stack
 * @param stack_size size of thread's stack
 * @returns ID of newly created thread, or special value THREAD_TABLE_FULL if there
 * is no more room for new threads.
 */
Thread_ID_t os_thread_create(void * entrypoint, void * data, uint32_t * stack, uint32_t stack_size);

/** Structure which holds information about existing threads
 * Implementer has to provide this variable having non-zero size
 */
extern struct OS_thread_t os_threads[];

/** Callback from kernel to calling code.
 * Implementer has to implement this function. It will provide kernel
 * with the information, how many threads there can be in os_threads
 * table allocated by the calling code.
 * @returns amount of slots available in os_threads array
 */
extern uint8_t kernel_threads_count();
