/*
 * sandbox.h
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_SANDBOX_H_
#define SOURCE_SANDBOX_H_

#include <stdbool.h>
#include <stdint.h>
#include "metadata.h"
#include "rb_tree/rb_tree.h"

struct sandbox {
    struct rb_tree indices;
    const uint8_t *base;
    const uint8_t *limit;
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

bool HARM_SandBox_PutObject(Object_t *object, const unsigned align_bits);

Object_t *HARM_SandBox_GetObject(const uint32_t address);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_SANDBOX_H_ */
