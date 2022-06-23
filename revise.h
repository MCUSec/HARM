/*
 * revise.h
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_REVISE_H_
#define SOURCE_REVISE_H_

#include <stdint.h>

enum _revise_type {
    R_DIRECT_BRANCH = 0,
    R_TABLE_BRANCH,

    __R_REVISE_TYPE_LIMIT
};

typedef enum _revise_type ReviseType_T;


struct _revise_item {
    uint16_t offset;
    uint32_t data;
} __attribute__((packed));

typedef struct _revise_item revise_item_t;


#endif /* SOURCE_REVISE_H_ */
