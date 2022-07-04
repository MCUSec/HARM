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

#ifndef METADATA_H
#define METADATA_H

#include "utils/rb_tree.h"
#include "macros.h"

struct branchinfo {
    uint16_t s_offset;
    union {
        uint32_t data;
        struct {
            uint16_t i_offset;
            uint16_t d_index;
        };
    } dst;
} PACKED;


struct object {
    struct rb_node *node;
    BranchInfo_t *b_info;
    uint32_t *entry;
    uint32_t address;
    uint16_t n_branch;
    uint16_t size;
};

union callsite {
    uint32_t v;
    struct {
        uint16_t offset;
        uint16_t caller_i;
    };
};

typedef struct object Object_t;
typedef struct branchinfo BranchInfo_t;
typedef union callsite Callsite_t;


#endif /* METADATA_H */