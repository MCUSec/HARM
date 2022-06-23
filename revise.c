/*
 * revise.c
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "harm.h"
#include "object.h"
#include "revise.h"

#define OPCODE_ASSERT(addr, code, mask, expected) { \
	unsigned short _opcode = (code) & (mask); \
	if (_opcode != (expected)) { \
		printf("%s (Line %d): unexpected opcode %x @ 0x%08x\r\n", __func__, __LINE__, _opcode, addr); \
		abort(); \
	} \
}

static uint32_t encode_B_T4(const uint32_t src_addr, const uint32_t dst_addr, uint32_t *mem)
{
    const int32_t offset = dst_addr - src_addr - 4;
    const uint32_t S = offset < 0;
    const uint32_t I1 = offset & (1 << 23) ? 1 : 0;
    const uint32_t I2 = offset & (1 << 22) ? 1 : 0;
    const uint32_t imm10 = (offset >> 12) & 0b1111111111;
    const uint32_t imm11 = (offset >> 1) & 0b11111111111;
    const uint32_t J1 = ((~I1 & 1UL) ^ S) & 1UL;
    const uint32_t J2 = ((~I2 & 1UL) ^ S) & 1UL;

    uint32_t code;

    code = 0b11110 << 27;
    code |= (S << 26);
    code |= (imm10 << 16);
    code |= (1 << 15);
    code |= (J1 << 13);
    code |= (1 << 12);
    code |= (J2 << 11);
    code |= imm11;

    *mem = ((code & 0xFFFFUL) << 16) | (code >> 16);

    return code;
}

static uint32_t encode_B_T3(const uint32_t src_addr, const uint32_t dst_addr, const uint8_t cc, uint32_t *mem)
{
    const int32_t offset = dst_addr - src_addr - 4;
    const uint32_t S = offset < 0;
    const uint32_t J1 = offset & (1 << 18) ? 1 : 0;
    const uint32_t J2 = offset & (1 << 19) ? 1 : 0;
    const uint32_t imm6 = (offset >> 12) & 0b111111;
    const uint32_t imm11 = (offset >> 1) & 0b11111111111;

    uint32_t code;

    code = 0b11110 << 27;
    code |= (S << 26);
    code |= ((cc & 0b1111) << 22);
    code |= (imm6 << 16);
    code |= (1 << 15);
    code |= (J1 << 13);
    code |= (J2 << 11);
    code |= imm11;

    *mem = ((code & 0xFFFFUL) << 16) | (code >> 16);

    return code;
}

static void direct_branch_revise(instance_t *instance, const revise_item_t *item)

{
    uint32_t target_index = (item->data >> 16) & 0xFFFFUL;
    uint32_t inner_offset = item->data & 0xFFFFUL;

    const object_t *target_obj = harm_get_object_by_index(target_index);

    uint32_t dst_addr = target_obj->instance->address + inner_offset;
    uint32_t src_addr = instance->address + item->offset;
    uint32_t src_code = *(uint32_t *)src_addr;

    if (!(src_code & (1 << 28))) {
        uint8_t cc = (src_code >> 6) & 0b1111;
        encode_B_T3(src_addr, dst_addr, cc, (uint32_t *)src_addr);
    } else {
        encode_B_T4(src_addr, dst_addr, (uint32_t *)src_addr);
    }
}

void object_revise(object_t *object)

{
    int i, revise_count = OBJECT_REVISE_COUNT(object);

    for (i = 0; i < revise_count; i++) {
        direct_branch_revise(object->instance, &object->revise_items[i]);
    }
}
