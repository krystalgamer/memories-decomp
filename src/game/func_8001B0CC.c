/* Scratchpad C stores and official Psy-Q GTE macros reproduce the projection
 * block through the RTPS-normalizing profile. */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "screen_projection.h"
#include "view_state.h"
#include "duel_screen_tables.h"

/* Projects one duel-slot coordinate pair and returns its signed screen X. */
s16 func_8001B0CC(s32 index)
{
    s32 coord = index;
    ProjectedPair result;

    SetGeomScreen(D_800F2848.projection);
    coord *= 4;
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    {
        SVECTOR *scratch = (SVECTOR *)0x1F8003E0;
        u16 *source = (u16 *)((u8 *)D_800908A0 + coord);
        u16 x = source[0];

        scratch->vy = -0x18;
        scratch->vx = x;
        scratch->vz = source[1];
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&result);
    }
    SetGeomOffset(0, 0);
    return result.x;
}
