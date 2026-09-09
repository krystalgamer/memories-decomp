#include "../types.h"
#include "display_object.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "duel_card.h"

void func_8001D240(DisplayObject *o)
{
    if (!func_80042B98((DisplayObjectLifecycle *)o)) {
        o->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if (!o->field_20.b.field_22) {
            *(s16 *)&o->position.h.field_28 = -8;
            o->position.h.field_2A = 0xC0;
        } else {
            o->position.h.field_28 = 8;
            o->position.h.field_2A = 0;
        }
    }
    o->field_20.b.field_22 += (s16)o->position.h.field_28;
    if (o->field_20.b.field_22 < 0xC0) {
        DuelCardRecord *e = &D_801A7AD8[o->field_6A];

        e->flags |= DUEL_CARD_FLAG_DEFENSE_POSITION;
        o->field_20.b.field_22 = (s16)o->position.h.field_2A;
        if (!o->field_20.b.field_22) {
            e->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
            if (!o->field_20.b.field_21) {
                o->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            }
        }
        o->field_6C = 0;
        o->update = 0;
    }
}
