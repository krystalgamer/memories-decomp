/* Scratchpad C stores and official Psy-Q GTE macros reproduce the projection
 * block. The compiler profile normalizes Psy-Q's RTPS placeholder opcode. */
#include "../types.h"
#include "duel_side_state.h"
#include "display_object.h"
#include "display_projection.h"
#include "screen_projection.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "func_80016784.h"
#define ORDERING_TABLE_SLOT2_ARRAY
#include "ordering_tables.h"

void func_80015D18(DisplayObject *object)
{
    SetGeomScreen(0x12C);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    {
        u8 *scratch = (u8 *)0x1F8003E0;
        u16 x = *(u16 *)((u8 *)object + 0x28);

        *(s16 *)(scratch + 2) = 0;
        *(u16 *)scratch = x;
        *(u16 *)(scratch + 4) = *(u16 *)((u8 *)object + 0x2A);
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy((u8 *)object + 0x30);
    }
    object->field_30.h.field_30 -= 0x20;
    object->field_30.h.field_32 -= 0x1E;
    SetGeomOffset(0, 0);
}

/* Immediately follows func_80015DB8 in the image and shares its projection
   setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
   swc2 $14. Here the source coordinates come from the object's own target
   record and the projected pair is biased into the object's screen fields. */
