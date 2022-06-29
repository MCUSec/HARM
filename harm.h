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
