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

#include <stdlib.h>
#include <time.h>
#include "harm.h"
#include "hal.h"

volatile uint32_t g_systick_count;

void HARM_HAL_SecureTimer_Init(void)
{
    g_systick_count = HARM_SHUFFLE_PERIOD_MS;

    NVIC_SYSTICK_CSR = 0UL;
    NVIC_SYSTICK_CVR = 0UL;
    NVIC_SYSTICK_RVR = (SystemCoreClock / 1000u) - 1UL;
    NVIC_SYSTICK_CSR = NVIC_SYSTICK_CLK_Msk | NVIC_SYSTICK_INT_Msk | NVIC_SYSTICK_ENA_Msk;
}

WEAK void HARM_HAL_SecureRNG_Init(void)
{
    srand(0);
}

WEAK int HARM_HAL_SecureRNG_GetNext(void)
{
    return rand();
}

WEAK void HARM_HAL_Device_Init(void)
{
}