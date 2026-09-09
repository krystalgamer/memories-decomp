#include "../types.h"
#include "display_object.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "duel_card.h"

void func_80019BD0(DisplayObject *o)
{
    int current;

    if (!func_80042B98((DisplayObjectLifecycle *)o)) {
        o->field_2C.h.field_2E = 0x40;
        o->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if (o->field_20.b.field_21) {
            o->field_2C.h.field_2E = 0xC0;
        }
    }
    current = o->field_20.b.field_21 + 8;
    o->field_20.b.field_21 = current;
    if (o->field_6C & 0x40) {
        if ((signed char)current >= 0) {
            o->field_20.b.field_21 = 0;
            D_801A7AD8[o->field_6A].flags &= ~DUEL_CARD_FLAG_DISPLAY_MARKER;
            if ((o->field_20.b.field_22 | o->field_20.b.field_20) == 0) {
                o->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            }
            o->field_6C = 0;
            o->update = 0;
        }
    } else if (o->field_20.b.field_21 >= o->field_2C.h.field_2E) {
        o->field_6C |= 0x40;
        o->field_20.b.field_21 = 0xC0;
        o->field_67 = 0;
    }
}
