#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include "macros.h"

#define SCB_NS_VTOR		*((volatile uint32_t *) 0xE002ED08UL)
#define SCB_NS_SCR 		*((volatile uint32_t *) 0xE002ED10UL)
#define SCB_SCR 		*((volatile uint32_t *) 0xE000ED10UL)
#define SCB_SCR_SLEEPDEEP_Pos 2U
#define SCB_SCR_SLEEPDEEP_Msk (1UL << SCB_SCR_SLEEPDEEP_Pos)


#define NVIC_SYSTICK_CSR		*((volatile uint32_t *) 0xE000E010UL)
#define NVIC_SYSTICK_RVR		*((volatile uint32_t *) 0xE000E014UL)
#define NVIC_SYSTICK_CVR		*((volatile uint32_t *) 0xE000E018UL)

#define NVIC_SYSTICK_CLK_Pos	2U
#define NVIC_SYSTICK_CLK_Msk	(1UL << NVIC_SYSTICK_CLK_Pos)

#define NVIC_SYSTICK_INT_Pos	1U
#define NVIC_SYSTICK_INT_Msk	(1UL << NVIC_SYSTICK_INT_Pos)

#define NVIC_SYSTICK_ENA_Pos	0U
#define NVIC_SYSTICK_ENA_Msk	(1UL << NVIC_SYSTICK_ENA_Pos)

#define __NVIC_PRIO_BITS        3

extern volatile uint32_t g_systick_count;
extern uint32_t SystemCoreClock;

#ifdef __cplusplus
extern "C" {
#endif

static inline void HARM_HAL_SecureTimer_Suspend(void)
{
    NVIC_SYSTICK_CSR = 0UL;
}

static inline void HARM_HAL_SecureTimer_Resume(void)
{
    NVIC_SYSTICK_CSR = NVIC_SYSTICK_CLK_Msk | NVIC_SYSTICK_INT_Msk | NVIC_SYSTICK_ENA_Msk;
}

void HARM_HAL_SecureTimer_Init(void);

void HARM_HAL_SecureRNG_Init(void) WEAK;

int HARM_HAL_SecureRNG_GetNext(void) WEAK;

#ifdef __cplusplus
}
#endif

#endif /* HAL_H */
