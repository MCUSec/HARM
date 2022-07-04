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
#include <string.h>
#include "sandbox.h"

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

SandBox_t g_sandbox;

static inline int HARM_SandBox_IndexCmp(const struct rb_node *node, const unsigned long key)
{
    const Object_t *object = (Object_t *)node->data;
    
    if (key == *object->entry ||
        (key > *object->entry &&
         key < *object->entry + object->size)) {
        return 0;
    } else if (key >= *object->entry + object->size) {
        return 1;
    } else {
        return -1;
    }
}

static void *HARM_SandBox_BlockAlloc(const size_t size, const unsigned align_bits)
{
    const size_t cap = (size_t)(g_sandbox.limit - g_sandbox.ptr);
    const uintptr_t block_base = ALIGN((uintptr_t)g_sandbox.ptr, 1 << align_bits);
    const size_t len = block_base - (uintptr_t)g_sandbox.ptr + size;

    if (len > cap) {
        return NULL;
    }

    g_sandbox.ptr += len;

    return (void *)block_base;
}

bool HARM_SandBox_PutObject(const Object_t *object, const unsigned align_bits)
{
    void *block = HARM_SandBox_BlockAlloc(object->size, align_bits);

    if (!block) {
        /* No enough space in sandbox */
        return false;
    }

    /* Copy object from flash memory to sandbox */
    memcpy(block, (void *)object->address, object->size);

    /* Update address */
    *object->entry = (uint32_t)block;

    rb_tree_node_put(&g_sandbox.indices, (uint32_t)block, object->node);

    return true;
}

Object_t *HARM_SandBox_GetObject(const uint32_t address)
{
    struct rb_node *rb_node = rb_tree_node_get(&g_sandbox.indices, address);
    if (rb_node) {
        return (struct object *)rb_node->data;
    }

    return NULL;
}

void HARM_SandBox_Init(void *base, const size_t length)
{
    g_sandbox.base = (uint8_t *)base;
    g_sandbox.limit = g_sandbox.base + length;
    g_sandbox.indices.root = NULL;
    g_sandbox.indices.compare = HARM_SandBox_IndexCmp;
}
