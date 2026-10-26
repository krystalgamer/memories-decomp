#ifndef MEMORIES_DECOMP_MODEL_UPDATE_COORDINATE_UNIT_MATRIX_H
#define MEMORIES_DECOMP_MODEL_UPDATE_COORDINATE_UNIT_MATRIX_H

#include "../types.h"
#include "../psyq/libgte.h"

struct _GsCOORDUNIT;

/* Rebuilds a coordinate unit's local matrix from its Euler angles, scales it
 * when `scale` is non-null, and clears the stamp so libgs recomputes the world
 * matrix on the next pass. Declared on the incomplete type so callers that
 * only forward a slot's root_coordinate do not have to pull in the libhmd chain. */
void Model_UpdateCoordinateUnitMatrix(struct _GsCOORDUNIT *unit, VECTOR *scale);

#endif
