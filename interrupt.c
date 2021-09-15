/*
 * interrupt.c
 *
 *  Created on: Jul 12, 2021
 *      Author: jiameng
 */

#include <stdint.h>
#include "config.h"

__attribute__((naked)) void SecureFault_Handler(void)
{
	__asm volatile(
	"  .syntax unified                         \n"
	"  .extern instanceList                    \n"
	"  .extern g_numInstances                  \n"
	"  .extern branchList                      \n"
	"  .extern g_numBranches                   \n"
	"                                          \n"
	"  tst     lr, #64                         \n"
	"  bne     _infinite_loop                  \n"
	"  tst     lr, #4                          \n"
	"  ite     ne                              \n"
	"  mrsne   r0, psp_ns                      \n"
	"  mrseq   r0, msp_ns                      \n"
	"  ldr     r1, [r0, #24]                   \n"
	"  lsrs    r2, r1, #28                     \n"
	"  cmp     r2, #3                          \n"
	"  beq     _dispatch_backward_ptr          \n"
	"  cmp     r2, #1                          \n"
	"  bne     _infinite_loop                  \n"
	"  mov     r2, r1                          \n"
	"  ubfx    r1, r2, #16, #12                \n"
	"  bfc     r2, #16, #16                    \n"
	"  b       _process_dispatch               \n"
	"                                          \n"
	"_dispatch_backward_ptr:                   \n"
	"  ubfx    r1, r1, #1, #27                 \n"
	"  ldr     r2, branch_table_size           \n"
	"  ldr     r2, [r2]                        \n"
	"  cmp     r1, r2                          \n"
	"  bge     _infinite_loop                  \n"
	"  ldr     r2, branch_table                \n"
	"  ldr     r2, [r2, r1, lsl #2]            \n"
	"  ubfx    r1, r2, #16, #16                \n"
	"  bfc     r2, #16, #16                    \n"
	"                                          \n"
	"_process_dispatch:                        \n"
	"  ldr     r3, dispatch_table_size         \n"
	"  ldr     r3, [r3]                        \n"
	"  cmp     r1, r3                          \n"
	"  bge     _infinite_loop                  \n"
	"  ldr     r3, dispatch_table              \n"
	"  ldr     r3, [r3, r1, lsl #2]            \n"
	"  adds    r2, r2, r3                      \n"
	"  orrs    r2, r2, #1                      \n"
	"  str     r2, [r0, #24]                   \n"
	"  bx      lr                              \n"
	"                                          \n"
	"_infinite_loop:                           \n"
	"  cpsid i                                 \n"
	"  bkpt #0                                 \n"
	"  b .                                     \n"
	"                                          \n"
	"  .align 2                                \n"
	"dispatch_table:      .long instanceList   \n"
	"dispatch_table_size: .long g_instance_num \n"
	"branch_table:        .long branchList     \n"
	"branch_table_size:   .long g_branch_num   \n"
	);
}

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
