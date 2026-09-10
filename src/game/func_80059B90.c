#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "model_update_view_metrics.h"

#define HMD_COORD_UNIT_OFFSET sizeof(SVECTOR)
#define HMD_MATRIX_OFFSET \
    (HMD_COORD_UNIT_OFFSET + sizeof(GsCOORDUNIT))
#define HMD_SCRATCH_SIZE (HMD_MATRIX_OFFSET + sizeof(MATRIX))

void func_80059B90(s16 value, s16 *out)
{
    /* One block preserves the retail SVECTOR/GsCOORDUNIT/MATRIX stack order. */
    u8 scratch[HMD_SCRATCH_SIZE];
    u16 result;

    memset(scratch, 0, sizeof(SVECTOR));
    ((SVECTOR *)scratch)->vx = value;
    func_800580D4(
        0,
        15,
        scratch,
        (GsCOORDUNIT *)(scratch + HMD_COORD_UNIT_OFFSET)
    );
    GsGetLwUnit(
        (GsCOORDUNIT *)(scratch + HMD_COORD_UNIT_OFFSET),
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    );
    result = *(volatile u16 *)&(
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    )->t[0];
    out[0] = result;
    result = *(volatile u16 *)&(
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    )->t[1];
    out[1] = result;
    result = *(volatile u16 *)&(
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    )->t[2];
    out[3] = 0;
    out[2] = result;
}
