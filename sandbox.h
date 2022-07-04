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

#ifndef SANDBOX_H
#define SANDBOX_H

#include <stdbool.h>
#include <stdint.h>
#include "metadata.h"
#include "utils/rb_tree.h"

struct sandbox {
    struct rb_tree indices;
    uint8_t *base;
    uint8_t *limit;
    uint8_t *ptr;
};

typedef struct sandbox SandBox_t;

#ifdef __cplusplus
extern "C" {
#endif

extern SandBox_t g_sandbox;


static inline void HARM_SandBox_Reset(void)
{
    g_sandbox.indices.root = NULL;
    g_sandbox.ptr = g_sandbox.base;
}

void HARM_SandBox_Init(void *base, const size_t length);

bool HARM_SandBox_PutObject(const Object_t *object, const unsigned align_bits);

Object_t *HARM_SandBox_GetObject(const uint32_t address);

#ifdef __cplusplus
}
#endif

#endif /* SANDBOX_H */
