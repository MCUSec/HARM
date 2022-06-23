/*
 * hal.h
 *
 *  Created on: Jul 13, 2021
 *      Author: jiameng
 */

#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>

#include "config.h"
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


extern volatile uint32_t g_systick_counter;
extern uint32_t SystemCoreClock;


#ifdef __cplusplus
extern "C" {
#endif


static inline void __hal_enable_systick(void)
{
    NVIC_SYSTICK_CSR = NVIC_SYSTICK_CLK_Msk | NVIC_SYSTICK_INT_Msk | NVIC_SYSTICK_ENA_Msk;
}

static inline void __hal_disable_systick(void)
{
    NVIC_SYSTICK_CSR = 0UL;
}

static inline void __hal_init_systick(void)
{
    g_systick_counter = RANDOM_PERIOD;

    NVIC_SYSTICK_CSR = 0UL;
    NVIC_SYSTICK_CVR = 0UL;
    NVIC_SYSTICK_RVR = (SystemCoreClock / SYSTICK_RATE_HZ) - 1UL;
    NVIC_SYSTICK_CSR = NVIC_SYSTICK_CLK_Msk | NVIC_SYSTICK_INT_Msk | NVIC_SYSTICK_ENA_Msk;
}


static inline void __hal_set_ns_vector_offset(uint32_t offset)
{
    SCB_NS_VTOR = offset;
}

void hal_rng_init(void);

uint32_t hal_rng_get_next(void);


#ifdef __cplusplus
}
#endif

#endif /* HAL_H_ */
