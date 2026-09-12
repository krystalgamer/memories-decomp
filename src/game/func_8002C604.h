#ifndef MEMORIES_DECOMP_FUNC_8002C604_H
#define MEMORIES_DECOMP_FUNC_8002C604_H

#include "../types.h"

/* Allocates and initializes one request for effect id, or returns 0 when the
 * pool has no free entry. */
u8 *func_8002C604(s32 id);

#endif
