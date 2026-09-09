#include "../types.h"
#include "display_object.h"
#include "duel_effect.h"
#include "text_box_runtime.h"

/* Fits the text box's frame object, held at 0x2C, around its content object
   at 0x28. Both are DisplayObjects, and the frame's six x/y pairs are the
   stride-8 run at 0x28, 0x30, 0x38, 0x40, 0x48 and 0x50 -- the same six
   DuelEffect_UpdateObjectLayout writes, and the second witness for reading
   that range as pairs.

   The six are a two-by-three grid, not a rectangle's four corners. Taking
   the content's position as (cx, cy) and its 0x3C/0x3E pair as its extent,
   the run comes out as

     0x28 (cx,     cy - 8)          0x30 (cx + w,     cy - 8)
     0x38 (cx - 8, cy + 0x4A)       0x40 (cx + w + 8, cy + 0x4A)
     0x48 (cx,     cy + h + 8)      0x50 (cx + w,     cy + h + 8)

   -- two columns of three, with the top row raised eight, the bottom row
   dropped eight, and the middle row pushed out eight on each side. The
   middle row's y comes off the content's 0x4A rather than its 0x3E, so it
   is not simply the halfway point. */
void func_80039140(DuelEffectChannel *record)
{
    DisplayObject *content = (DisplayObject *)record->field_28;
    DisplayObject *frame = (DisplayObject *)record->field_2C;
    u16 v;

    frame->field_18 = content->field_18;
    frame->field_1A = content->field_1A;

    v = content->field_30.h.field_30;
    frame->field_48.h.field_48 = v;
    frame->position.h.field_28 = v;
    frame->field_38.h.field_38 = v - 8;

    v = content->field_30.h.field_30 + content->field_3C.h.field_3C;
    frame->field_50.h.field_50 = v;
    frame->field_30.h.field_30 = v;
    frame->field_40.h.field_40 = v + 8;

    v = content->field_30.h.field_32 - 8;
    frame->field_30.h.field_32 = v;
    frame->position.h.field_2A = v;

    v = content->field_30.h.field_32 + content->field_48.h.field_4A;
    frame->field_40.h.field_42 = v;
    frame->field_38.h.field_3A = v;

    v = content->field_30.h.field_32 + content->field_3C.h.field_3E + 8;
    frame->field_50.h.field_52 = v;
    frame->field_48.h.field_4A = v;
}
