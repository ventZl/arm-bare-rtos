Nano RTOS
=========

This is "bare" RTOS. Smallest-possible Cortex-M thread switcher you can write.
It does only do the heavy lifting needed to perform thread switching and lets the
user to decide on everything else. It doesn't even implement any specific
scheduling model, so in fact you can extend it and have whatever scheduler
model, either preemptive, or cooperative; round robin or prioritized RT
scheduling.

Resulting in small memory and flash footprint allows you to have the luxury of
running threads even in more advanced flash bootloaders. It also has barely any
dependencies. All that this code needs is a CMSIS header to grab some structs
out of it. It should therefore be a quick drop-in addition to any project where
thread switching is needed.

Usage
-----

To actually use nano RTOS, all you have to do is to use the contents of
[src/kernel] subdirectory. There is one C file and a bunch of headers. Cloning
this repository as a submodule will work too. This repository does not have any
submodules defined and contains only small "dead" code in examples. Everything
else here is entirely optional.

Aside from that, you'll have to provide a thread table variable and one
call-back method, which provides kernel with amount of available entries in the 
thread table.

Documentation
-------------

The whole nano RTOS API is described by the [src/kernel/api.h] file. It contains
of one struct, one variable forward declaration and five functions.

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

Example
-------

In [examples] subdirectory, there is an example of how to use the kernel to
create an environment with preemptive multi-threading. More examples to come.
