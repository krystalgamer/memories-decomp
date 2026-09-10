#ifndef MEMORIES_DECOMP_FUNC_8005922C_H
#define MEMORIES_DECOMP_FUNC_8005922C_H

#include "../types.h"

struct _GsCOORDUNIT;

/* Rebuilds a coordinate unit's local matrix from its Euler angles, scales it
 * when `scale` is non-null, and clears the stamp so libgs recomputes the world
 * matrix on the next pass. Declared on the incomplete type so callers that
 * only forward a slot's field_D18 do not have to pull in the libhmd chain. */
void func_8005922C(struct _GsCOORDUNIT *unit, void *scale);

#endif
