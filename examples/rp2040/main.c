#include <kernel/api.h>
#include <pico/stdlib.h>
#include <RTE_Components.h>
#include CMSIS_device_header
#include <stdint.h>

#define THREADS_MAX         16

uint8_t kernel_threads_count() { return THREADS_MAX; }

struct OS_thread_t os_threads[THREADS_MAX];

uint32_t thread1_stack[256];
uint32_t thread2_stack[256];

const uint32_t LED_PIN = PICO_DEFAULT_LED_PIN;

/// Our example systick handler 
// This implements round robin scheduler. It goes around thread table
// starting with next thread after one currently running. It searches
// for whatever thread which is marked as ready. It then schedules this 
// thread for execution and schedules PendSV interrupt handler to be called.
void SysTick_Handler()
{
    Thread_ID_t current_thread = os_get_current_thread();
    Thread_ID_t next_thread = (current_thread + 1) % kernel_threads_count();
    do {
        if (os_threads[next_thread].state == THREAD_STATE_READY)
        {
            os_schedule_context_switch(next_thread);
            return;
        }
        next_thread = (next_thread + 1) % kernel_threads_count();
    } while (next_thread != current_thread);
}

// Example thread #1. It blindly activates our RPi Pico only LED
void thread1_main(void * data)
{
    (void) data;

    while (1) {
        gpio_put(LED_PIN, 1);
    }
}

// Example thread #2. It blindly deactivated our RPi Pico only LED
void thread2_main(void * data)
{
    (void) data;
    while (1) {
        gpio_put(LED_PIN, 0);
    }
}

/// Systic setup routine.
// This routine takes systick interval in milliseconds and generates
// systick reload value. If this is too large for current CPU speed
// then it is trimmed to largest possible value. Next, systick is
// initialized using this value.
// Next, we set PendSV priority to 255 (actually 192) so it is one of
// the lowerst-priority interrupts and never interrupts anything else 
// than regular threads.
void kernel_tick_setup(int interval_ms)
{
    const uint32_t systick_max = (1 << 24) - 1;
    uint32_t systick_val = (SystemCoreClock / 1000) * interval_ms;
    if (systick_val > systick_max)
        systick_val = systick_max;
    // It seems that some implementations make SysTick of lower priority than other interrupts
    // Here we need that PendSV has absolutely the lowest priority, so it will never interrupt 
    // any other ISR
    NVIC_SetPriority(PendSV_IRQn, 255);
    SysTick_Config(systick_val);
}

/// Firmware entrypoint
// Here we set generic GPIO properties, create two threads and start the RTOS
int main(void)
{
    // RPi Pico LED configuration - make GPIO an output
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Create two threads, specify their entrypoints, their stacks, and use no entrypoint arguments
    Thread_ID_t thr1 = os_thread_create(thread1_main, (void *) 0, thread1_stack, sizeof(thread1_stack));
    Thread_ID_t thr2 = os_thread_create(thread2_main, (void *) 0, thread2_stack, sizeof(thread2_stack));
    
    // Setup systick
    kernel_tick_setup(500);

    // Start up the kernel
    os_start(thr1);

    while (1);    
}
