#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../game/screen_projection.h"
#include "../game/view_state.h"
#include "../game/duel_screen_tables.h"

s16 func_8001B0CC(s32 index)
{
    s32 coord = index;
    s16 result;
    u16 value;
    u16 next_value;
    volatile s16 *scratch;
    volatile u16 *source;

    SetGeomScreen(D_800F2848.projection);
    coord *= 4;
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    scratch = (volatile s16 *)0x1F8003E0;
    source = (volatile u16 *)((u8 *)D_800908A0 + coord);
    value = source[0];
    scratch[1] = -0x18;
    scratch[0] = value;
    next_value = source[1];
    scratch[2] = next_value;
    gte_ldv0((s16 *)scratch);
    gte_rtps();
    gte_stsxy(&result);
    SetGeomOffset(0, 0);
    return result;
}
