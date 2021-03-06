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

#include "harm.h"
#include "sandbox.h"
#include "hal.h"

static struct rb_node rb_nodes[HARM_OBJECT_LIST_SIZE];

static uint16_t s_rand_seq[HARM_OBJECT_LIST_SIZE];

void HARM_ShuffleObjects(ExceptionFrame_t *excepframe)
{
    Object_t *suspended_fn = NULL;
    int16_t suspended_offset = -1;
    int i;

    if (excepframe) {
        suspended_fn = HARM_SandBox_GetObject(excepframe->pc);
        suspended_offset = suspended_fn ? *suspended_fn->entry - excepframe->pc : -1;
    }

    HARM_SandBox_Reset();

    /* Get a shuffled sequence */
    for (i = 0; i < HARM_OBJECT_LIST_SIZE; i++) {
        int j = HARM_HAL_SecureRNG_GetNext() % (i + 1);
        int t = s_rand_seq[i];
        s_rand_seq[i] = s_rand_seq[j];
        s_rand_seq[j] = t;
    }

    for (i = 0; i < HARM_OBJECT_LIST_SIZE; i++) {
        const int j = s_rand_seq[i];
        unsigned align_bits;

        if (&g_objects[j] == g_ns_vec_tbl) {
            align_bits = 16;
        } else {
            if ((g_objects[j].address & ~3) == g_objects[j].address) {
                align_bits = 2;
            } else {
                align_bits = 1;
            }
        }

        if (!HARM_SandBox_PutObject(&g_objects[j], align_bits)) {
            /* ERROR: no enough space in sandbox !!! */
            HARM_OnError(HARM_ERROR_NO_MEMORY);
        }
    }

    HARM_Object_FixRefs();

    if (suspended_fn && suspended_offset != -1) {
        /* Store return address of exception */
        excepframe->pc = *suspended_fn->entry + suspended_offset;
    }
}


void HARM_Bootstrap(void)
{
    funcptr_ns ns_entry;

    HARM_ShuffleObjects(NULL);

    HARM_HAL_SecureTimer_Init();

    ns_entry = (funcptr_ns)*g_ns_vectors[1]->entry;
    ns_entry();

    /* Never reach here */
    for (;;);
}

void HARM_OnError(const int errcode)
{
    __disable_irq();

    HARM_ErrorHandler(errcode);
    
    for (;;);
}

WEAK void HARM_ErrorHandler(const int errcode)
{
    UNUSED(errcode);
}
