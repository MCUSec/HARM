/*
 * shuffleable.h
 *
 *  Created on: Sep 13, 2020
 *      Author: jiameng
 */

#ifndef SOURCE_OBJECT_H_
#define SOURCE_OBJECT_H_

#include "revise.h"

struct _object_instance {
	uint32_t address;
};

typedef struct _object_instance instance_t;

struct _callsite {
	union {
		uint32_t value;
		uint16_t offset;
		uint16_t index;
	} address;
	instance_t *callee;
};

typedef struct _callsite callsite_t;

struct _object {
	instance_t	*instance;
	uint32_t	address;
	uint32_t	flags;
	uint16_t 	size;
} __attribute__((packed));

typedef struct _object object_t;

#define OBJECT_TYPE_POS 			(0)
#define OBJECT_TYPE_MASK 			(0x1UL)

#define OBJECT_IS_ISR_POS 			(1)
#define OBJECT_IS_ISR_MASK 			(0x1UL)

#define OBJECT_IRQ_POS 				(2)
#define OBJECT_IRQ_MASK 			(0xFFUL)

#define OBJECT_REVISE_COUNT_POS		(10)
#define OBJECT_REVISE_COUNT_MASK 	(0x3FFFFFUL)

#define OBJECT_TYPE(o) 			(((o)->flags >> OBJECT_TYPE_POS) & OBJECT_TYPE_MASK)
#define OBJECT_IS_ISR(o) 		(((o)->flags >> OBJECT_IS_ISR_POS) & OBJECT_IS_ISR_MASK)
#define OBJECT_IRQ(o) 			(((o)->flags >> OBJECT_IRQ_POS) & OBJECT_IRQ_MASK)
#define OBJECT_REVISE_COUNT(o)	(((o)->flags >> OBJECT_REVISE_COUNT_POS) & OBJECT_REVISE_COUNT_MASK)

#endif /* SOURCE_OBJECT_H_ */
