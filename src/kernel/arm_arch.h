#pragma once

/* If we are running GCC, then try to include CMSIS RTE header to derive location
 * of the device file.
 */
#if defined __has_include
#  if __has_include(<RTE_Components.h>)
#    include <RTE_Components.h>
#  endif
#endif

/* If location of device file has been specified (using any way) then try to include
 * it. If not, then check for nano-rtos specific define. If neither is specified, then
 * trip an error.
 */
#ifdef CMSIS_device_header
#include CMSIS_device_header

#define SCB_ICSR SCB->ICSR
#define SCB_ICSR_PENDSVSET (1 << SCB_ICSR_PENDSVSET_Pos)
#define cortex_enable_interrupts() __disable_irq()
#define cortex_disable_interrupts() __enable_irq()

#else
#  ifdef kernel_device_header
#  include kernel_device_header
#  else
#  error "You have to provide path either to CMSIS device file, or to file which provides alternatives to CMSIS intrinsics"
#  endif
#endif
#include <stdint.h>

#ifdef __STATIC_FORCEINLINE
/* CMSIS has this useful define, use it if it is available */
#define ALWAYS_INLINE __STATIC_FORCEINLINE
#else
#define ALWAYS_INLINE static inline
#endif

#ifdef __ARM_ARCH_6M__

/** SW implementation of what ISR returns really does */
ALWAYS_INLINE void __ISR_return()
{
		asm volatile(
				"POP {R0, R1}\n\t"
				"MOV R12, R0\n\t"
				"MOV LR, R1\n\t"
				"POP {R0, R1, R2, R3}\n\t"
				"POP {R6, R7}\n\t"
				"BX R6\n\t"
				);
}

#else

// TODO: Test this
ALWAYS_INLINE void __ISR_return()
{
		asm volatile(
				"POP {R0, R1, R2, R3, R12, LR}\n\t"
				"POP {R0, R1, R2, R3}\n\t"
				"POP {R6, R7}\n\t"
				"BX R6\n\t"
				);
}

#endif

/** Save application context.
 * This function will grab process SP
 * This operation will claim 32 bytes (8 registers * 4 bytes) on stack.
 * @return top of application stack after application context was saved
 */
ALWAYS_INLINE uint32_t * save_context()
{
	uint32_t * scratch;
	asm (
			".syntax unified\n\t"
			"MRS %0, PSP\n\t"
			"SUBS %0, #16\n\t"
			"STMEA %0!, {r4 - r7}\n\t"
			"SUBS %0, #32\n\t"
			"MOV r4, r8\n\t"
			"MOV r5, r9\n\t"
			"MOV r6, r10\n\t"
			"MOV r7, r11\n\t"
			"STMEA %0!, {r4 - r7}\n\t"
			"SUBS %0, #16\n\t"
			: "=r" (scratch)
			:
			: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
	);

	return scratch;
}

/** Load application context saved by save_context
 * from address sp.
 * @param sp address where top of the stack containing application context is
 */
ALWAYS_INLINE void load_context(uint32_t * sp)
{
	asm (
			"LDMFD %0!, {r4 - r7}\n\t"
			"MOV r8, r4\n\t"
			"MOV r9, r5\n\t"
			"MOV r10, r6\n\t"
			"MOV r11, r7\n\t"
			"LDMFD %0!, {r4 - r7}\n\t"
			"MSR PSP, %0\n\t"
			:
			: [scratch] "r" (sp)
			: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
	);
}

static inline void schedule_pendsv()
{
	SCB_ICSR |= SCB_ICSR_PENDSVSET;
}


