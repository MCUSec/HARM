/*
 * macros.h
 *
 *  Created on: Oct 19, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_MACROS_H_
#define SOURCE_MACROS_H_

#include <stdint.h>

#define ASM						__attribute__((naked))
#define NONSECURE_CALLABLE		__attribute__((cmse_nonsecure_entry))
#define PACKED					__attribute__((packed))
#define USED					__attribute__((used))
#define WEAK					__attribute__((weak))

#if 1

#define __BKPT(x) 				__asm volatile("bkpt #" # x)
#define __disable_irq() 		__asm volatile("cpsid i")
#define __enable_irq() 			__asm volatile("cpsie i")
#define __DSB() 				__asm volatile("dsb 0xF" ::: "memory")
#define __DMB() 				__asm volatile("dmb 0xF" ::: "memory")
#define __ISB() 				__asm volatile("isb 0xF" ::: "memory")
#define __WFI()					__asm volatile("wfi")

#endif







#if 1
__attribute__((always_inline)) static inline void __set_BASEPRI(uint32_t basePri)
{
	__asm volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}
#endif

#endif /* SOURCE_MACROS_H_ */
