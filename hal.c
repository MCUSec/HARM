/*
 * hal.c
 *
 *  Created on: Jul 14, 2021
 *      Author: jiameng
 */
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "LPC55S69_cm33_core0.h"
#include "fsl_rng.h"

#define RNG_PRESENT 1

void hal_rng_init(void)
{
#if defined(RNG_PRESENT) && RNG_PRESENT
	RNG_Init(RNG);
#else
	srand(time(NULL));
#endif
}

uint32_t hal_rng_get_next(void)
{
#if defined(RNG_PRESENT) && RNG_PRESENT
	uint32_t data;
	status_t status = RNG_GetRandomData(RNG, &data, 4);
	assert(status == kStatus_Success);
	return data;
#else
	return rand();
#endif
}
