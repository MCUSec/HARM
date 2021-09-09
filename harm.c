/*
 * core.c
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "harm.h"
#include "object.h"
#include "revise.h"
#include "revise_list.h"
#include "object_list.h"
#include "branch_list.h"
#include "sandbox.h"
#include "rb_tree/rb_tree.h"
#include "hal.h"


#define TARGET_FW_TOTAL_OBJECTS (sizeof(instanceList) / sizeof(instanceList[0]))
#define TARGET_FW_TOTAL_BRANCHES (sizeof(branchList) / sizeof(branchList[0]))
#define SANDBOX_COUNT 1


struct harm harm_core;


__attribute__((section(".data.$GLOBAL_REGION")))
static struct rb_node rb_nodes[TARGET_FW_TOTAL_OBJECTS];

static uint16_t s_rand_seq[TARGET_FW_TOTAL_OBJECTS];

const int g_instance_num = TARGET_FW_TOTAL_OBJECTS;
const int g_branch_num = TARGET_FW_TOTAL_BRANCHES;

volatile uint32_t g_systick_counter;

static inline void swap(uint16_t *a, uint16_t *b)
{
	uint16_t t = *a;
	*a = *b;
	*b = t;
}

static void sequence_shuffle(uint16_t *seq_arr, const int length)
{
	int i;

	for (i = length - 1; i > 0; i--) {
		int j = hal_rng_get_next() % (i + 1);
		swap(&seq_arr[i], &seq_arr[j]);
	}
}


static int rb_tree_node_compare(const struct rb_node *node, const unsigned long key)
{
	const object_t *object = node->data;
	if (key == object->instance->address ||
			(key > object->instance->address &&
					key < object->instance->address + object->size)) {
		return 0;
	} else if (key >= object->instance->address + object->size) {
		return 1;
	} else {
		return -1;
	}
}


static void vector_update_entry(int index, uint32_t address)
{
	assert(index > 0 && index <= 0xFF);
	uint32_t *vector = (uint32_t *)(harm_core.objects[0].instance->address);
	vector[index] = address | 1UL;
}

static void do_randomize(uint32_t *pc)
{
	uint32_t orig_addr, new_addr, current_offset = 0, ipsr;

	extern void object_revise(const object_t *object);
	sandbox_t *sandbox = harm_core.sandbox;

	const object_t *current_object = NULL;
	struct rb_node *rb_node = NULL;
	uint16_t i, j;

	__asm volatile("mrs %0, IPSR" : "=r"(ipsr));

	if (ipsr != 0) {
		assert(pc != NULL);
		current_object = harm_get_object_by_address(*pc);
		if (current_object) {
			current_offset = *pc - current_object->instance->address;
		}
	}

	sandbox_reset(sandbox);
	sequence_shuffle(&s_rand_seq[1], TARGET_FW_TOTAL_OBJECTS - 1);

	for (i = 0; i < harm_core.num_objects; i++) {
		j = s_rand_seq[i];

		rb_node = &rb_nodes[j];
		rb_node->data = &harm_core.objects[j];

		if (ipsr != 0) {
			orig_addr = harm_core.objects[j].instance->address;
		} else {
			orig_addr = harm_core.objects[j].address;
		}

		if (j == 0) {
			// Object 0 is the vector
			new_addr = (uint32_t)(uintptr_t)sandbox_bucket_allocate(sandbox, harm_core.objects[j].size, 128, rb_node);
		} else if (orig_addr == (orig_addr & ~3UL)) {
			new_addr = (uint32_t)(uintptr_t)sandbox_bucket_allocate(sandbox, harm_core.objects[j].size, 4, rb_node);
		} else {
			new_addr = (uint32_t)(uintptr_t)sandbox_bucket_allocate(sandbox, harm_core.objects[j].size, 2, rb_node);
		}

		if (!new_addr) {
			printf("[SECURE][ERROR] Object %d: Out of memory!!\r\n", i);
			abort();
		}

		if (j != 0) {
			memcpy((void *)new_addr, (void *)harm_core.objects[j].address, harm_core.objects[j].size);
		} else {
			*(uint32_t *)new_addr = *(uint32_t *)harm_core.objects[0].address;
		}

		harm_core.objects[j].instance->address = (uint32_t)new_addr;
	}

	for (i = 1; i < harm_core.num_objects; i++) {
		object_revise(&harm_core.objects[i]);
		if (OBJECT_IS_ISR(&harm_core.objects[i])) {
			vector_update_entry(OBJECT_IRQ(&harm_core.objects[i]),
								harm_core.objects[i].instance->address | 0x1UL);
		}
	}

	if (current_object && pc) {
		*pc = current_offset + current_object->instance->address;
	}

	__hal_set_ns_vector_offset(harm_core.objects[0].instance->address);
}

const object_t *harm_get_object_by_index(int index)
{
	return &harm_core.objects[index];
}

const object_t *harm_get_object_by_address(uint32_t address)
{
	const object_t *object = sandbox_get_object(harm_core.sandbox, address);
	return object;
}

int harm_get_object_index(const object_t *object)
{
	if (object) {
		return (int)(object - &harm_core.objects[0]);
	}

	return -1;
}


void harm_randomize_code(uint32_t *stack_pointer)
{
	uint32_t pc, ipsr;

	__asm volatile("mrs %0, IPSR" : "=r"(ipsr));
	if (ipsr == 0) {
		return;
	}

	printf("triggered\r\n");

	__set_BASEPRI(3 << (8 - __NVIC_PRIO_BITS));

	pc = stack_pointer[6] & ~1UL;
	do_randomize(&pc);

	stack_pointer[6] = pc | 1UL;

	__DSB();
	__ISB();

	__set_BASEPRI(0);
}

void harm_start(void)
{
	__hal_init_systick();
}

void harm_init(harm_config_t *config)
{
	int i;

	harm_core.config = config;
	harm_core.objects = objectList;
	harm_core.revise_items = reviseItems;

	harm_core.num_objects = sizeof(objectList) / sizeof(objectList[0]);
	harm_core.num_revise_items = sizeof(reviseItems) / sizeof(reviseItems[0]);
	harm_core.sandbox = sandbox_create((uint32_t)config->sandbox.base, config->sandbox.size, rb_tree_node_compare);
	assert(harm_core.sandbox);

	for (i = 0; i < harm_core.num_objects; i++) {
		s_rand_seq[i] = i;
	}

	printf("[SECURE] Number of objects: %d\r\n", harm_core.num_objects);
	printf("[SECURE] Number of revise items: %d\r\n", harm_core.num_revise_items);

	hal_rng_init();

	do_randomize(NULL);

	harm_start();
}


