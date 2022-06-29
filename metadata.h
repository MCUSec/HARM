#ifndef METADATA_H
#define METADATA_H

#include "rb_tree/rb_tree.h"
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