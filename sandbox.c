/*
 * sandbox.c
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */


#include <stdlib.h>
#include <string.h>
#include "sandbox.h"

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

struct _sandbox {
    struct rb_tree rb_tree;
    uint32_t size;
    uint32_t bufPtr;
    uint32_t base;
    uint32_t used;
    uint32_t capacity;
};


sandbox_t *sandbox_create(const uint32_t start, const size_t size, node_comp_cb_t compare)
{
    sandbox_t *sandBox = malloc(sizeof(struct _sandbox));

    sandBox->rb_tree.root = NULL;
    sandBox->rb_tree.compare = compare;

    sandBox->base = start;
    sandBox->used = 0;
    sandBox->bufPtr = start;
    sandBox->capacity = size;
    sandBox->size = size;

    return sandBox;
}

void sandbox_destroy(sandbox_t **sandBox)
{
    if (*sandBox) {
        free(*sandBox);
        *sandBox = NULL;
    }
}

void sandbox_reset(sandbox_t *sandBox)
{
    sandBox->rb_tree.root = NULL;
    sandBox->bufPtr = sandBox->base;
    sandBox->used = 0;
    sandBox->capacity = sandBox->size;
}

void *sandbox_bucket_allocate(sandbox_t *sandBox, const size_t size, const unsigned align, struct rb_node *rb_node)
{
    uint32_t newBase = sandBox->bufPtr;
    unsigned int len;

    if (align == 2) {
        newBase = newBase | 0x2UL;
    } else {
        newBase = ALIGN(newBase, align);
    }

    len = newBase - sandBox->bufPtr + size;

    if (sandBox->capacity > len) {
        sandBox->capacity -= len;
        sandBox->used += len;
        sandBox->bufPtr += len;
        if (rb_node) {
            rb_tree_node_put(&sandBox->rb_tree, newBase, rb_node);
        }
    } else {
        newBase = 0;
    }

    return (void *)newBase;
}

void *sandbox_get_object(sandbox_t *sandBox, const uint32_t address)
{
    struct rb_node *rb_node = rb_tree_node_get(&sandBox->rb_tree, address);
    if (rb_node) {
        return (void *)rb_node->data;
    }

    return NULL;
}

