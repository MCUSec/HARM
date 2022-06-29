#ifndef MACROS_H
#define MACROS_H

#include <stdint.h>

#define ASM						__attribute__((naked))
#define NONSECURE_CALLABLE		__attribute__((cmse_nonsecure_entry))
#define PACKED					__attribute__((packed))
#define USED					__attribute__((used))
#define WEAK					__attribute__((weak))
#define PACKED                  __attribute__((packed)) 
#define NORETURN                __attribute__((noreturn))
#define EXPORT                  __attribute__((visibility("default")))
#define UNUSED(x)               (void)x
#if 1

#define __BKPT(x) 				__asm volatile("bkpt #" # x)
#define __disable_irq() 		__asm volatile("cpsid i")
#define __enable_irq() 			__asm volatile("cpsie i")
#define __DSB() 				__asm volatile("dsb 0xF" ::: "memory")
#define __DMB() 				__asm volatile("dmb 0xF" ::: "memory")
#define __ISB() 				__asm volatile("isb 0xF" ::: "memory")
#define __WFI()					__asm volatile("wfi")
#define __HANDLER_MODE() \
({ \
    uint32_t __ipsr; \
    __asm volatile("mrs %0, IPSR" : "=r"(__ipsr) : : "memory"); \
    __ipsr != 0; \
})

#define __get_MSP_NS() \
({ \
    uint32_t __msp_ns; \
    __asm volatile("mrs %0, MSP_NS" : "=r"(__msp_ns) : : "memory"); \
    __msp_ns; \
})

#define __get_PSP_NS() \
({ \
    uint32_t __psp_ns; \
    __asm volatile("mrs %0, PSP_NS" : "=r"(__psp_ns) : : "memory"); \
    __psp_ns; \
})


#endif







#if 1
__attribute__((always_inline)) static inline void __set_BASEPRI(uint32_t basePri)
{
    __asm volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}
#endif

#endif /* MACROS_H */
