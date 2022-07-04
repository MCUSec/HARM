/**
 * MIT License
 *
 * Copyright (c) 2022 MCUSec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include "CMSIS/core_cm33.h"
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

void HARM_HAL_Device_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_H */
