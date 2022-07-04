#include "metadata.h"
#include "harm.h"

const Object_t g_objects[HARM_OBJECT_LIST_SIZE];

const Callsite_t g_callsite_tbl[HARM_CALLSITE_TBL_SIZE];

const Object_t *g_ns_vectors[1];

const Object_t *g_ns_vec_tbl;

uint32_t g_dispatch_tbl[HARM_OBJECT_LIST_SIZE];
