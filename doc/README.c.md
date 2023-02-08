Use of C-based implementation of nano RTOS
==========================================

To use pure C version of the code, you need the following files:
 * src/kernel/api.h
 * src/kernel/kernel.c 
 * src/kernel/kernel.h 
 * src/kernel/arm_arch.h

The whole nano RTOS API is described by the [src/kernel/api.h](src/kernel/api.h) file. 
It contains one struct, one variable forward declaration and five functions.

As a user, you will most probably only need to deal with some of them:

~~~~~
Thread_ID_t os_thread_create(void * entrypoint, void * data, uint32_t * stack,
uint32_t stack_size)
~~~~~

Serves the purpose of creating new thread. It is your responsibility to allocate
stack space for the thread and provide it together with the address of entrypoint 
function. Optionally you can provide one void pointer, which will be passed as
the first argument to the entrypoint function. Threads are normally configured
to run as unprivileged. Returns thread ID usable by the `os_start` or 
`os_schedule_context_switch` or special value to signal that there is no more
room in thread table.

~~~~~
void os_start(Thread_ID_t startup_thread)
~~~~~

This will actually start the thread switcher and jump into startup_thread. You
shall ever only call this function once. It will never return and will pass
control to the specified thread, using the stack which was provided for the
thread.

~~~~
bool os_schedule_context_switch(Thread_ID_t next_thread_id)
~~~~

This routine will schedule thread switch. It has to be called from a privileged
context, which means that the thread itself cannot call this function directly.
It has to be wrapped either in SVC call (yield, or cooperative multi-threading), 
or called from some timer handler (preemptive multi-threading). Actual thread
switch is done using PendSV interrupt service routine. This opens the
possibility of doing thread switches from IRQ service routines. Yet one has to 
implement some locking as the routine is not re-entrant as of now.

~~~~~
uint8_t kernel_threads_count()
struct OS_Thread_t os_threads[];
~~~~~

These two are something you have to provide. You have to allocate the `os_threads`
array containing as many slots for threads as your application needs. Then, you
have to provide `kernel_threads_count()` function, which nano RTOS can use to
find out the end of the thread table.
