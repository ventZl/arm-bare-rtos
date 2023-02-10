#include "kernel.h"
#include "api.h"
#include <stdint.h>

#include <stdbool.h>

static struct OS_core_state_t core_state[CPU_CORE_COUNT];

static bool ctxt_switch_pending;

bool os_schedule_context_switch(struct OS_Thread_t * next_thread)
{
    const uint8_t cpu = kernel_current_cpu();
    struct OS_Thread_t * current_thread = core_state[cpu].thread_current;

	if (current_thread->state == THREAD_STATE_RUNNING)
	{
		// only mark leaving thread as ready, if it was runnig before
		// if leaving thread was, for example, quit before calling
		// os_sched_yield, then this would return it back to life
		current_thread->state = THREAD_STATE_READY;
	}
	core_state[cpu].thread_next = next_thread;

	next_thread->state = THREAD_STATE_RUNNING;

    schedule_pendsv();

	__ISB();
	__DSB();

	return true;
}

/** Handle task switch.
 * This function performs the heavy lifting of context switching
 * when CPU is switched from one task to another.
 * As of now, it stores outgoing task's application context onto stack
 * and restores incoming task's context from its stack.
 * It then sets PSP to point to incoming task's stack and resumes
 * normal operation.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
	/* Do NOT put anything here. You will clobber context being stored! */
	asm volatile(
			".syntax unified\n\t"
			"push {lr}\n\t"
            "cpsid i\n\t"
	);
//	cortex_disable_interrupts();
	/* Do NOT put anything here. You will clobber context being stored! */
    uint32_t * temp_sp = save_context();

    /* Save SP of outgoing process into os_threads */
    const uint8_t cpu = kernel_current_cpu();
    struct OS_Thread_t * current_thread = core_state[cpu].thread_current;
    current_thread->sp = temp_sp;

    /* Load SP of incoming process from os_threads */
    struct OS_Thread_t * next_thread = core_state[cpu].thread_next;
    temp_sp = next_thread->sp;

//    core_state[cpu].thread_prev = current_thread;
    core_state[cpu].thread_current = next_thread;

	ctxt_switch_pending = false;
	load_context(temp_sp);
	/* Do NOT put anything here. You will clobber context just restored! */
//	cortex_enable_interrupts();
	asm volatile(
            "cpsie i\n\t"
			"pop {pc}\n\t"
	);
}

void __attribute__((noreturn)) os_start(struct OS_Thread_t * startup_thread)
{
    uint8_t cpu = kernel_current_cpu();
    core_state[cpu].thread_current = startup_thread;
    startup_thread->state = THREAD_STATE_RUNNING;

    // Start this thread
    // We are adding 8 here, because normally pend_sv_handler would be reading 8 general 
    // purpose registers here. But there is nothing useful there, so we simply skip it.
    // Code below then restores what would normally be restored by return from handler.
    uint32_t * thread_sp = startup_thread->sp + 8;
    __set_PSP((uint32_t) thread_sp);
    __set_CONTROL(0x03); 	// SPSEL = 1 | nPRIV = 1: use PSP and unpriviledged thread mode

    __ISB();

    __ISR_return();
}

/** Default catch for the case when thread exits its main()
 * This function will perform empty loop. This is just for the case
 * that any thread manages to return from it's entrypoint function.
 * This function is defined weak, you can override it by your implementation
 * if you want to do something more sophisticated.
 */
void __attribute__((noreturn, weak)) os_thread_dispose()
{
    while (1) {};
}

int os_thread_create(struct OS_Thread_t * thread, void (* entrypoint)(void*), void * data, uint8_t * stack, uint32_t stack_size)
{
    if (thread != NULL)
    {
        if (thread->state == THREAD_STATE_EMPTY) {
            thread->sp = (uint32_t *) &stack[stack_size - sizeof(struct Frame)];

            // Place where thread state frame begins on thread's stack
            struct Frame * isr_frame = (struct Frame *) thread->sp;

            isr_frame->r0 = (uint32_t) data;
            isr_frame->lr = (uint32_t) os_thread_dispose;
            isr_frame->pc = (unsigned long) entrypoint;
            isr_frame->psr = 0x01000000; // Activate just Thumb mode - mandatory

            thread->state = THREAD_STATE_READY;
            return OS_E_OK;
        }
        else {
            return OS_E_BUSY;
        }
    }

    return OS_E_NULLPTR;
}

struct OS_Thread_t * os_get_current_thread()
{
    uint8_t cpu = kernel_current_cpu();
    struct OS_Thread_t * current_thread = core_state[cpu].thread_current;
    return current_thread;
}
