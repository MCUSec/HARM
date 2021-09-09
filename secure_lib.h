/*
 * secure_api.h
 *
 *  Created on: Sep 26, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_SECURE_API_H_
#define SOURCE_SECURE_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void call_address_dispatch(void);

uint32_t PendSV_hook0(uint32_t ulLR);

void return_address_dispatch(void);

void benchmark_start(void);

void benchmark_stop(void);

uint32_t benchmark_get_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_SECURE_API_H_ */
