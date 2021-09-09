/*
 * secure_lib.c
 *
 *  Created on: Jul 13, 2021
 *      Author: jiameng
 */

#include "harm.h"
#include "hal.h"

extern struct harm harm_core;


WEAK USED void __dispatch_fail(void) {
	__disable_irq();
	while (1) {
		__BKPT(0);
	}
}

USED static uint32_t address_encode(uint32_t address)
{
	uint32_t index, offset, ret = address;
	const object_t *object;

	object = harm_get_object_by_address(address);

	if (object) {
		index = harm_get_object_index(object);
		offset = address - object->instance->address;
		ret = ((0x1000UL | index) << 16) | offset;
	}

	return ret;
}

ASM NONSECURE_CALLABLE uint32_t PendSV_hook0(uint32_t ulLR)
{
	__asm volatile(
	"  .syntax unified                           \n"
	"  .extern __acle_se_return_address_dispatch \n"
	"                                            \n"
	"  push    {r3-r4, r7, lr}                   \n"
	"  sub     sp, #16                           \n"
	"  add     r7, sp, #0                        \n"
	"  mov     r3, r0                            \n"
	"  str     r3, [r7, #8]                      \n"
	"  tst     r3, #64                           \n"
    "  bne     process_secure_context            \n"
	"  tst     r3, #4                            \n"
	"  ite     ne                                \n"
	"  mrsne   r3, psp_ns                        \n"
	"  beq     PendSV_hook_ret                   \n"
	"  str     r3, [r7, #4]                      \n"
	"  ldr     r0, [r3, #24]                     \n"
	"  bl      address_encode                    \n"
	"  orr     r0, r0, #1                        \n"
	"  ldr     r3, [r7, #4]                      \n"
	"  str     r0, [r3, #24]                     \n"
	"  ldr     r0, [r3, #20]                     \n"
	"  bl      address_encode                    \n"
	"  ldr     r3, [r7, #4]                      \n"
	"  str     r0, [r3, #20]                     \n"
	"  b       PendSV_hook_ret                   \n"
	"                                            \n"
	"process_secure_context:                     \n"
	"  tst     r3, #4                            \n"
	"  ite     ne                                \n"
	"  mrsne   r3, psp                           \n"
	"  beq     PendSV_hook_ret                   \n"
	"  str     r3, [r7, #4]                      \n"
	"  ldr     r0, [r3, #60]                     \n"
	"  bl      address_encode                    \n"
	"  ldr     r3, [r7, #4]                      \n"
	"  str     r0, [r3, #60]                     \n"
	"                                            \n"
	"PendSV_hook_ret:                            \n"
	"  ldr     r0, [r7, #8]                      \n"
	"  add     sp, #16                           \n"
	"  pop     {r3-r4, r7}                       \n"
	"  ldr     lr, [sp], #4                      \n"
    "  b    __acle_se_return_address_dispatch    \n"

	);
}

ASM NONSECURE_CALLABLE void return_address_dispatch(void)
{
	__asm volatile(
	"  .syntax unified                             \n"
	"  .extern instanceList                        \n"
	"  .extern branchList                          \n"
	"                                              \n"
	"  push   {r0-r3}                              \n"
	"  ubfx   r0, lr, #1, #27                      \n"  // r0 = lr[27:1] (return_index)
	"  ldr    r1, branch_table                     \n"  // r1 = branch_table
	"  ldr    r1, [r1, r0, lsl #2]                 \n"  // r1 = branch_table[r0]
	"  uxth   r0, r1                               \n"  // r0 = r1[15:0]  (return offset)
	"  lsr    r1, r1, #16                          \n"  // r1 = r1[31:16] (caller index)
	"  ldr    r2, dispatch_table                   \n"  // r2 = dispatch_table
	"  ldr    r2, [r2, r1, lsl #2]                 \n"
	"  add    lr, r2, r0                           \n"  // lr = r2 + r0
	"  pop    {r0-r3}                              \n"
	"  bxns   lr                                   \n"
	"                                              \n"
	"  .align 2                                    \n"
	"branch_table: .word branchList                \n"
	"dispatch_table: .word instanceList            \n"
	);
}

ASM NONSECURE_CALLABLE void call_address_dispatch(void)
{
	__asm volatile(
	"  .syntax unified                            \n"
	"  .extern instanceList                       \n"
	"  .extern g_instance_num                     \n"
	"                                             \n"
	"  push    {r0-r1}                            \n"
	"  tst     r12, #0x10000000                   \n"
	"  beq     __dispatch_fail                    \n"
    "  ubfx    r12, r12, #16, #12                 \n"  // r12 = r12[27:16] (index)
	"  ldr     r0, dispatch_table_size2           \n"
	"  ldr     r0, [r0]                           \n"
	"  cmp     r12, r0                            \n"
	"  bge     __dispatch_fail                    \n"
	"  ldr     r0, dispatch_table2                \n"
	"  ldr     r12, [r0, r12, lsl #2]             \n"
	"  pop     {r0-r1}                            \n"
	"  bxns    r12                                \n"
	"                                             \n"
	"  .align 2                                   \n"
	"dispatch_table2: .word instanceList          \n"
	"dispatch_table_size2: .word g_instance_num   \n"
	);
}

ASM NONSECURE_CALLABLE void enter_low_power_mode(void)
{
	__asm volatile(
	"  push   {r3-r4}                             \n"
	"  cpsid  i                                   \n"
	"  ldr    r3, =0xe000e010                     \n"
	"  ldr    r4, [r3]                            \n"
	"  bic    r4, #1                              \n"
	"  str    r4, [r3]                            \n"
	"  wfi                                        \n"
	"  orrs   r4, r4, #1                          \n"
	"  str    r4, [r3]                            \n"
	"  cpsie  i                                   \n"
	"  pop    {r3-r4}                             \n"
	"  b    __acle_se_return_address_dispatch     \n"
	"  .align 2                                   \n"
	: : : "memory"
	);
}
