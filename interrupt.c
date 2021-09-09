/*
 * interrupt.c
 *
 *  Created on: Jul 12, 2021
 *      Author: jiameng
 */

#include <stdint.h>
#include "config.h"


__attribute__((naked)) void PendSV_Handler(void)
{
	__asm volatile(
	"  .syntax unified                 \n"
	"  .extern harm_randomize_code     \n"
	"                                  \n"
	"  tst     lr, #8                  \n"
	"  it      eq                      \n"
	"  bxeq    lr                      \n"
	"  tst     lr, #64                 \n"
	"  it      ne                      \n"
    "  bxne    lr                      \n"
	"  tst     lr, #4                  \n"
	"  ite     eq                      \n"
	"  mrseq   r0, msp_ns              \n"
	"  mrsne   r0, psp_ns              \n"
	"  b       harm_randomize_code     \n"
	);
}


#if 0
__attribute__((naked)) void SysTick_Handler(void)
{
	__asm volatile(
	"  .syntax unified                       \n"
	"  .extern g_systick_counter             \n"
	"                                        \n"
	"  cpsid i                               \n"
	"  ldr   r0, .L_systick_cnt              \n"
	"  ldr   r1, [r0]                        \n"
	"  subs  r1, #1                          \n"
	"  cbnz  r1, .L_return                   \n"
	"  ldr   r2, .L_int_ctrl                 \n"
	"  ldr   r3, [r2]                        \n"
	"  orrs  r3, r3, #0x10000000             \n"
	"  str   r3, [r2]                        \n"
	"  mov   r1, %0                          \n"
	".L_return:                              \n"
	"  str   r1, [r0]                        \n"
	"  cpsie i                               \n"
	"  bx    lr                              \n"
	"                                        \n"
	"  .align 2                              \n"
	".L_systick_cnt: .long g_systick_counter \n"
	".L_int_ctrl:    .long 0xe000ed04        \n"
	:: "i" (RANDOM_PERIOD) : "memory"

	);
}
#else

extern volatile uint32_t g_systick_counter;

void SysTick_Handler(void)
{

	g_systick_counter++;
	if (g_systick_counter % RANDOM_PERIOD == 0) {
		printf("triggered\r\n");
	}
}
#endif
