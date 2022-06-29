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

ASM void HARM_PendSVHandler_Hook(void)
{
    __asm volatile(
        "  .syntax unified                       \n"
        "  .extern HARM_ShuffleObjects           \n"
        "                                        \n"
        "  tst     lr, #8                        \n"
        "  it      eq                            \n"
        "  bxeq    lr                            \n"
        "  tst     lr, #64                       \n"
        "  it      ne                            \n"
        "  bxne    lr                            \n"
        "  tst     lr, #4                        \n"
        "  ite     eq                            \n"
        "  mrseq   r0, msp_ns                    \n"
        "  mrsne   r0, psp_ns                    \n"
        "  b       HARM_ShuffleObjects           \n"
    );
}

ASM void HARM_SysTickHandler_Hook(void)
{
    __asm volatile(
        "  .syntax unified                       \n"
        "  .extern g_systick_count               \n"
        "                                        \n"
        "  cpsid i                               \n"
        "  ldr   r0, =g_systick_count            \n"
        "  ldr   r1, [r0]                        \n"
        "  subs  r1, #1                          \n"
        "  cbnz  r1, .L_return                   \n"
        "  ldr   r2, =0xe000ed04                 \n"
        "  ldr   r3, [r2]                        \n"
        "  orrs  r3, r3, #0x10000000             \n"
        "  str   r3, [r2]                        \n"
        "  ldr   r1, =%0                         \n"
        ".L_return:                              \n"
        "  str   r1, [r0]                        \n"
        "  cpsie i                               \n"
        "  bx    lr                              \n"
        "                                        \n"
        "  .align 2                              \n"
        :: "i" (HARM_SHUFFLE_PERIOD_MS) : "memory"
    );
}

void HARM_SecureFaultHandler_Hook(void)
{
    __asm volatile("nop");
}