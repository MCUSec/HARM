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

#ifndef HARM_H
#define HARM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sandbox.h"
#include "arch.h"

#define HARM_ERROR_SUCCESS  0
#define HARM_ERROR_INDEX_OVERFLOW 1
#define HARM_ERROR_NOT_ISR  2
#define HARM_ERROR_NO_OBJECT    3
#define HARM_ERROR_NO_MEMORY    4

#ifndef HARM_OBJECT_LIST_SIZE  
#define HARM_OBJECT_LIST_SIZE   1
#endif

#ifndef HARM_VECTOR_TBL_SIZE
#define HARM_VECTOR_TBL_SIZE   1
#endif

#ifndef HARM_CALLSITE_TBL_SIZE
#define HARM_CALLSITE_TBL_SIZE 1
#endif

#ifndef HARM_SHUFFLE_PERIOD_MS
#define HARM_SHUFFLE_PERIOD_MS 200
#endif

extern const Object_t g_objects[];

extern const Object_t *g_ns_vectors[];

extern const Object_t *g_ns_vec_tbl;

/* typedef for non-secure callback functions */
typedef void (*funcptr_ns)(void) __attribute__((cmse_nonsecure_call));

#ifdef __cplusplus
extern "C" {
#endif

void HARM_OnError(const int errcode);

void HARM_ErrorHandler(const int errcode);

void HARM_Bootstrap(void) EXPORT;

#ifdef __cplusplus
}
#endif

#endif /* HARM_H */
