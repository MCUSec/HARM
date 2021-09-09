/*
 * sandbox.h
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_SANDBOX_H_
#define SOURCE_SANDBOX_H_

#include <stdint.h>
#include "rb_tree/rb_tree.h"

typedef struct _sandbox sandbox_t;

#ifdef __cplusplus
extern "C" {
#endif

sandbox_t *sandbox_create(const uint32_t base, const size_t size, node_comp_cb_t compare);
void sandbox_destroy(sandbox_t **sandBox);

void sandbox_reset(sandbox_t *sandBox);
void *sandbox_bucket_allocate(sandbox_t *sandBox, const size_t size, const unsigned align, struct rb_node *rb_node);

void *sandbox_get_object(sandbox_t *sandBox, const uint32_t address);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_SANDBOX_H_ */
