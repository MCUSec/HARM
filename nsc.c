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
#include "hal.h"

static uint32_t HARM_ISR_EncodeAddress(const uint32_t exc_return_val)
{
    ExceptionReturn_t exc_return = { .v = exc_return_val };
    ExceptionFrame_t *exc_frame;
    Object_t *object;
    uint32_t offset, index;

    if (!__HANDLER_MODE()) {
        HARM_OnError(HARM_ERROR_NOT_ISR);
    }

    if (exc_return.S == 1) {
        /* Inerrupt is taken in secure state */
        
        return exc_return_val;
    }

    exc_frame = exc_return.SPSEL == 1 ? __get_PSP_NS() : __get_MSP_NS();

    object = HARM_SandBox_GetObject(exc_frame->pc);
    if (!object) {
        HARM_OnError(HARM_ERROR_NO_OBJECT);
    }

    offset = exc_frame->pc - *object->entry;
    index = (uint32_t)(object - g_objects);
    exc_frame->pc = (index << 16) | offset;

    return exc_return_val;
}


ASM NONSECURE_CALLABLE void HARM_NSC_SecureReturn(void)
{
    __asm volatile(
        "  .syntax unified                             \n"
        "  .extern g_dispatch_tbl                      \n"
        "  .extern g_callsite_tbl                      \n"
        "                                              \n"
        "  push   {r0-r2}                              \n"
        "  ubfx   r0, lr, #1, #27                      \n"
        "  ldr    r1, =%0                              \n"
        "  cmp    r0, r1                               \n"
        "  itt    ge                                   \n"
        "  ldrge  r0, =%1                              \n"
        "  bge    HARM_OnError                         \n"
        "  ldr    r1, =g_callsite_tbl                  \n"
        "  ldr    r1, [r1, r0, lsl #2]                 \n"
        "  uxth   r0, r1                               \n"
        "  lsr    r1, r1, #16                          \n"
        "nsc_reentry_s1:                               \n"
        "  ldr    r2, =g_dispatch_tbl                  \n"
        "  ldr    r2, [r2, r1, lsl #2]                 \n"
        "  add    lr, r2, r0                           \n"
        "  pop    {r0-r2}                              \n"
        "  bxns   lr                                   \n"
        "nsc_reentry_e1:                               \n"
        "  .align 2                                    \n"
        : :"i"(HARM_CALLSITE_TBL_SIZE), "i"(HARM_ERROR_INDEX_OVERFLOW)
    );
}


ASM NONSECURE_CALLABLE void HARM_NSC_SecureReturnISR(void)
{
    __asm volatile(
        "  .syntax unified                             \n"
        "  .extern g_dispatch_tbl                      \n"
        "                                              \n"
        "  mrs     r0, ipsr                            \n"
        "  cmp     r0, #0                              \n"
        "  beq     HARM_OnError                        \n"
        "  tst     lr, #4                              \n"
        "  ite     ne                                  \n"
        "  mrsne   r0, psp_ns                          \n"
        "  mrseq   r0, msp_ns                          \n"
        "  ldr     r1, [r0, #24]                       \n"
        "  uxth    r3, r1                              \n"
        "  lsr     r1, r1, #16                         \n"
        "  ldr     r2, =%0                             \n"
        "  cmp     r1, r2                              \n"
        "  itt     ge                                  \n"
        "  ldrge   r0, =%1                             \n"
        "  bge     HARM_OnError                        \n"
        "nsc_reentry_s2:                               \n"
        "  ldr     r2, =g_dispatch_tbl                 \n"
        "  ldr     r2, [r2, r1, lsl #2]                \n"
        "  add     r2, r3                              \n"
        "  str     r2, [r0, #24]                       \n"
        "  dmb                                         \n"
        "  bxns      lr                                \n"
        "nsc_reentry_e2:                               \n"
        "  .align 2                                    \n"
        : :"i"(HARM_OBJECT_LIST_SIZE), "i"(HARM_ERROR_INDEX_OVERFLOW)
    );
}


ASM NONSECURE_CALLABLE void HARM_NSC_SecureIndirectCall(void)
{
    __asm volatile(
        "  .syntax unified                           \n"
        "  .extern g_dispatch_tbl                    \n"
        "  push    {r0-r1}                           \n"
        "  ldr     r0, =%0                           \n"
        "  mov     r1, r12                           \n"
        "  cmp     r1, r0                            \n"
        "  itt     ge                                \n"
        "  ldrge   r0, =%1                           \n"
        "  bge     HARM_OnError                      \n"
        "nsc_reentry_s3:                             \n"
        "  ldr     r0, =g_dispatch_tbl               \n"
        "  ldr     r12, [r0, r1, lsl #2]             \n"
        "  pop     {r0-r1}                           \n"
        "  bxns    r12                               \n"
        "nsc_reentry_e3:                             \n"
        "  .align  2                                 \n"
        : :"i"(HARM_OBJECT_LIST_SIZE), "i"(HARM_ERROR_INDEX_OVERFLOW)
    );
}


ASM NONSECURE_CALLABLE void HARM_NSC_SecureSleep(void)
{
    __asm volatile(
        "  .syntax unified                            \n"
        "  .extern HARM_SecureTimer_Suspend           \n"
        "  .extern HARM_SecureTimer_Resume            \n"
        "                                             \n"
        "  cpsid i                                    \n"
        "  push  {lr}                                 \n"
        "  bl    HARM_SecureTimer_Suspend             \n"
        "  wfi                                        \n"
        "  bl    HARM_SecureTimer_Resume              \n"
        "  ldr   lr, [sp], #4                         \n"
        "  cpsie i                                    \n"
        "  bxns  lr                                   \n"
    );
}


ASM NONSECURE_CALLABLE void HARM_NSC_SecureEnterISR(const uint32_t exc_return_val)
{
    __asm volatile(
        "  .syntax unified                           \n"
        "                                            \n"
        "  push    {lr}                              \n"
        "  bl      HARM_ISR_EncodeAddress            \n"
        "  ldr     lr, [sp], #4                      \n"
        "  bxns    lr                                \n"
    );
}
