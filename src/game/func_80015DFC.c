/* Volatile scratchpad stores and the second record reload preserve retail
 * ordering around the official Psy-Q RTPS macros. The scratchpad word is the
 * SVECTOR gte_ldv0 loads, and the qualifier lives on that pointer, so its
 * three stores are plain member stores. */
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

/* Immediately follows func_80015DB8 in the image and shares its projection
   setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
   swc2 $14. Here the source coordinates come from the object's own target
   record and the projected pair is biased into the object's screen fields. */

void func_80015DFC(DisplayProjectionTrackedObject *object)
{
    ProjectedPair p;
    s32 y;

    GsSetLsMatrix(&D_800FE148);
    {
        u16 x = object->record->field_30.h.field_30;
        volatile SVECTOR *scratch = (volatile SVECTOR *)0x1F8003E0;

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz =
            *(u16 *)&((volatile DisplayProjectionTrackedObject *)object)
                        ->record->field_34.h.field_34;
        gte_ldv0((u8 *)scratch);
        gte_rtps();
        gte_stsxy(&p);
    }
    object->screen_x = p.x - 0x1A;
    y = p.y;
    object->screen_y = y - 0x1E;
    if (D_8009B1D5 != 0) {
        if (object->field_18 < 0xF) {
            object->screen_y = y - 0x1D;
        }
    } else {
        if (object->field_18 >= 0xF) {
            object->screen_y = y - 0x1D;
        }
    }
    func_80016784(object->record, (s32)D_800E9D98[0], object->screen_x,
                  object->screen_y);
    SetGeomOffset(0xA0, 0x6C);
}
