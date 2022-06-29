#include "harm.h"
#include "metadata.h"
#include "hal.h"

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

static inline void direct_branch_revise(uint32_t obj_addr, const BranchInfo_t *binfo)
{
    const Object_t *target_obj = &g_objects[binfo->dst.d_index];
    const uint32_t dst_addr = *target_obj->entry + binfo->dst.i_offset;
    const uint32_t src_addr = obj_addr + binfo->s_offset;
    const uint32_t src_code = *(uint32_t *)src_addr;

    if (!(src_code & (1 << 28))) {
        uint8_t cc = (src_code >> 6) & 0b1111;
        encode_B_T3(src_addr, dst_addr, cc, (uint32_t *)src_addr);
    } else {
        encode_B_T4(src_addr, dst_addr, (uint32_t *)src_addr);
    }
}

void HARM_Object_FixRefs(void)
{
    uint32_t *ns_vectors = (uint32_t *)*g_ns_vec_tbl->entry;
    Object_t *obj_iter = &g_objects[0];
    int i = 0;

    for (; i < HARM_OBJECT_LIST_SIZE; i++, obj_iter++) {
        BranchInfo_t *binfo = obj_iter->b_info;
        uint32_t obj_entry = *obj_iter->entry;
        int j = 0;
        
        for (; j < obj_iter->n_branch; j++, binfo++) {
            direct_branch_revise(obj_entry, binfo);
        }
    }

    for (i = 1; i < HARM_VECTOR_TBL_SIZE; i++) {
        ns_vectors[i] = *g_ns_vectors[i]->entry;
    }

    __DSB();
    SCB_NS_VTOR = *g_ns_vec_tbl->entry;
}

