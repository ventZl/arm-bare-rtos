/* SPDX-License-Identifier: BSD-3-Clause
 *
 * A pure C++20 reimplementation of Eduard's Drusa arm-bare-rtos in a single module.
 * Some concepts borrowed from https://github.com/zyp/laks.
 *
 * Copyright (c) 2022, Marek Koza (qyx@krtko.org)
 * Copyright (c) 2023, Eduard Drusa
 * Copyright (c) 2013, Vegard Storheil Eriksen
 *
 * All rights reserved.
 */

module;

#include <cstdint>
#include <cstdlib>
#include <kernel/arm_arch.h>

export module bare_rtos;

export class Thread {
	public:
		enum State {
			/* Thread is ready to run, it may be scheduled at any time. */
			READY = 0,
			/* Thread is currently running. There is only a single running thread in the scheduler. */
			RUNNING,
			/* Suspended for whatever reason. The thread won't be scheduled until it is resumed. */
			SUSPENDED,
		} state;

		/* The current thread stack frame / saved context on the stack. */
		struct Frame {
			uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
			uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
		} *sp;

		/* Pointer to the next thread in a linked list (or any other data structure). */
		Thread *next = nullptr;

	private:
		/* Placeholder for threads returning from their main function. */
		static void dispose(void) {
			while (true) ;
		}

	public:
		Thread(void (*entrypoint)(void *), uint32_t *stack, size_t stack_size, void *data) {
			sp = (Frame *)((uint8_t *)stack + stack_size - sizeof(Frame));

			sp->r0 = (uint32_t)data;
			sp->lr = (uint32_t)dispose;
			sp->pc = (uint32_t)entrypoint;
			sp->psr = 0x01000000;
		}

		void suspend(void) {
			/* If this thread is running, we can safely suspend and yield. If we are suspending some other
			 * thread, suspend it without yielding. */
			/** @todo yield */
			state = SUSPENDED;
		}

		void resume(void) {
			/* The current running thread cannot be resumed. Keep it running. */
			if (state != RUNNING) {
				state = READY;
			}
		}
};


export class Scheduler {
	public:
		Thread *current_thread = nullptr;
		Thread *thread_list = nullptr;
		Thread *switch_next = nullptr;

		Scheduler(void) {}

		void start() {
			/* Always start with the first thread in the list (the last one added). */
			current_thread = thread_list;
			current_thread->state = Thread::State::RUNNING;

			/* Run the thread by resuming it's context. */
			__set_PSP((uint32_t)&(current_thread->sp->r0));
			__set_CONTROL(0x03); 	// SPSEL = 1 | nPRIV = 1: use PSP and unpriviledged thread mode
			__ISB();
			__ISR_return();
		}

		/**
		 * @brief Append thread @p t to the list of schedulable threads
		 */
		void append(Thread& t, Thread::State state = Thread::State::READY) {
			/* Save it to the front. */
			t.state = state;
			t.next = thread_list;
			thread_list = &t;
		}

		/**
		 * @brief Switch the context to a @p n thread
		 */
		void switchContext(Thread& n) {
			/* If the current thread state is RUNNING, the thread was preempted and is still
			 * ready to run. Do not manipulate the state otherwise (eg. for suspended threads). */
			if (current_thread->state == Thread::State::RUNNING) {
				current_thread->state = Thread::State::READY;
			}

			/* Save pointer to the thread we are switching context to and request pendsv. */
			switch_next = &n;
			switch_next->state = Thread::State::RUNNING;
			schedule_pendsv();
			__ISB();
			__DSB();
		}

		/**
		 * @brief A simple round robin scheduler
		 */
		void schedule(uint32_t elapsed = 1) {
			/* There is no thread running for whatever reason. */
			if (current_thread == nullptr) {
				return;
			}

			/* Find the next ready-to-run thread or the current running thread if there is nothing
			 * else ready-to-run. Note we are starting from the current thread. If we encounter
			 * a running thread, we have already reached the end of the list. */
			Thread *n = current_thread;
			do {
				n = n->next;
				if (!n) {
					n = thread_list;
				}
			} while (n->state != Thread::State::READY && n->state != Thread::State::RUNNING);
			switchContext(*n);
		}
};


export auto scheduler = Scheduler();

extern "C" __attribute__((naked)) void pend_sv_handler(void) {
	asm volatile(
		".syntax unified\n"
		"push {lr}\n"
		"cpsid i\n"
	);

	/* Save the old thread context. */
	uint32_t *temp_sp = save_context();
	scheduler.current_thread->sp = (Thread::Frame *)temp_sp;

	/* Resume the new thread context. */
	temp_sp = (uint32_t *)scheduler.switch_next->sp;
	scheduler.current_thread = scheduler.switch_next;
	scheduler.switch_next = nullptr;
	load_context(temp_sp);

	asm volatile(
		"cpsie i\n"
		"pop {pc}\n"
	);
}

