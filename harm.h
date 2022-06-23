/*
 * core.h
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_SHUFFLER_H_
#define SOURCE_SHUFFLER_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "object.h"
#include "sandbox.h"

#ifndef ENABLE_BENCHMARK
#define ENABLE_BENCHMARK 1
#endif

struct harm_config {
    struct {
        void *base;
        unsigned int size;
    } sandbox;
    uint32_t cpu_clock_hz;
    uint32_t systick_rate;
    uint32_t shuffle_period;
//	void (*vector_update)(void);
};

typedef struct harm_config harm_config_t;

struct harm {
    int	active_sandbox;
    int num_objects;
    int num_revise_items;
    sandbox_t *sandbox;
    const harm_config_t *config;
    const object_t *objects;
    const revise_item_t *revise_items;
};

extern volatile uint32_t g_systick_counter;
extern struct harm harm_core;

#ifdef __cplusplus
extern "C" {
#endif

const object_t *harm_get_object_by_index(int index);

const object_t *harm_get_object_by_address(uint32_t address);

int harm_get_object_index(const object_t *object);

void harm_init(harm_config_t *config);

void harm_randomize_code(uint32_t *ulSP);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_SHUFFLER_H_ */
