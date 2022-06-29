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