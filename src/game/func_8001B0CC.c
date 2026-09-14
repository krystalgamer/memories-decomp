/* Ordered scratchpad C stores and official Psy-Q GTE macros reproduce the
 * single-coordinate projection through the G0 RTPS profile. */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "screen_projection.h"
#include "view_state.h"
#include "duel_screen_tables.h"

/* Projects the coordinate pair at D_800908A0[index * 2] through the GTE and
   returns the projected screen X. The result is read back as a signed halfword,
   which is what makes the return type s16 rather than s32. */
s16 func_8001B0CC(s32 index)
{
    /* The scaled index survives three calls and then becomes the table's byte
       offset, naturally retaining the retail callee-saved register. */
    register s32 coord = index;
    ProjectedPair result;

    SetGeomScreen(D_800F2848.projection);
    coord = coord * 4;
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    {
        u8 *source;
        volatile u8 *scratch;
        u16 x;

        source = (u8 *)D_800908A0 + coord;
        scratch = (volatile u8 *)0x1F8003E0;
        x = *(u16 *)source;
        *(s16 *)(scratch + 2) = -0x18;
        *(u16 *)scratch = x;
        *(u16 *)(scratch + 4) = *(u16 *)(source + 2);
        gte_ldv0((u8 *)scratch);
        gte_rtps();
        gte_stsxy(&result);
    }
    SetGeomOffset(0, 0);
    return (s16)result.x;
}
