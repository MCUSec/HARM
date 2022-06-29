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