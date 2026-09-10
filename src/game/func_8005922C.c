#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "func_8005922C.h"

/* Rebuilds one coordinate unit's local matrix from its Euler angles, scales it
 * when the caller supplies a scale vector, and clears the stamp so libgs
 * recomputes the world matrix on the next pass. */
void func_8005922C(GsCOORDUNIT *unit, void *scale)
{
    RotMatrixYXZ_gte(&unit->rot, &unit->matrix);
    if (scale != 0) ScaleMatrix(&unit->matrix, scale);
    unit->flg = 0;
}
