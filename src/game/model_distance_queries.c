#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"

#include "model.h"
#include "model_distance_queries.h"

extern u8 D_800F3A10[];
/* NOT converted to the GsRVIEW2 in camera_view.h, and deliberately so.
   These six reads take the low halfword of vpx..vrz. Written as
   `*(u16 *)&D_800F56F0.vpx` the two functions come out four bytes short
   of retail, and through a `const u16 *` cursor they come out long --
   the raw array is the only one of the three that reproduces the target.
   The other nine users of the symbol do use the typed view. */
extern u16 D_800F56F0[];

s32 func_8005A1F4(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u8 *entry = D_800F3A10 + index * MODEL_SLOT_SIZE;

    memset(&difference, 0, 8);
    difference.vx = D_800F56F0[0] - *(u16 *)(entry + 0);
    difference.vy = D_800F56F0[2] - *(u16 *)(entry + 2);
    difference.vz = D_800F56F0[4] - *(u16 *)(entry + 4);
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}

s32 func_8005A2E0(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u8 *entry = D_800F3A10 + index * MODEL_SLOT_SIZE;

    memset(&difference, 0, 8);
    difference.vx = D_800F56F0[6] - *(u16 *)(entry + 0);
    difference.vy = D_800F56F0[8] - *(u16 *)(entry + 2);
    difference.vz = D_800F56F0[10] - *(u16 *)(entry + 4);
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}
