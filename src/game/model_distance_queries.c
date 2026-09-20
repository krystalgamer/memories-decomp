#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"

#include "model.h"
#define D_800F56F0_AS_HALFWORDS
#include "camera_view.h"
#include "model_distance_queries.h"

#define MODEL_SLOT_HALFWORDS(slots, index) \
    ((u16 *)((u8 *)(slots) + (index) * MODEL_SLOT_SIZE))

/* These six reads deliberately take camera_view.h's raw halfword arm rather
   than its GsRVIEW2 view. Written as
   `*(u16 *)&D_800F56F0.vpx` the two functions come out four bytes short
   of retail, and through a `const u16 *` cursor they come out long --
   the raw array is the only one of the three that reproduces the target.
   The other nine users of the symbol do use the typed view. */
s32 Model_GetSlotDistanceFromCameraEye(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(D_800F3A10, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = D_800F56F0[0] - entry[0];
    difference.vy = D_800F56F0[2] - entry[1];
    difference.vz = D_800F56F0[4] - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}

s32 Model_GetSlotDistanceFromCameraTarget(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(D_800F3A10, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = D_800F56F0[6] - entry[0];
    difference.vy = D_800F56F0[8] - entry[1];
    difference.vz = D_800F56F0[10] - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}
