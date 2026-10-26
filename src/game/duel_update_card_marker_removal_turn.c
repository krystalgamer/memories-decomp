#include "../types.h"
#include "display_object.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "duel_scene_card_placement.h"
#include "display_callback_transitions.h"

void Duel_UpdateCardMarkerRemovalTurn(DisplayObject *o)
{
    int current;

    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o)) {
        o->field_2C.h.field_2E = 0x40;
        o->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if (o->field_20.b.rotation_y) {
            o->field_2C.h.field_2E = 0xC0;
        }
    }
    current = o->field_20.b.rotation_y + 8;
    o->field_20.b.rotation_y = current;
    if (o->field_6C & 0x40) {
        if ((signed char)current >= 0) {
            o->field_20.b.rotation_y = 0;
            gDuel_aCardRecords[o->field_6A].flags &= ~DUEL_CARD_FLAG_DISPLAY_MARKER;
            if ((o->field_20.b.rotation_z | o->field_20.b.rotation_x) == 0) {
                o->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            }
            o->field_6C = 0;
            o->update = 0;
        }
    } else if (o->field_20.b.rotation_y >= o->field_2C.h.field_2E) {
        o->field_6C |= 0x40;
        o->field_20.b.rotation_y = 0xC0;
        o->field_67 = 0;
    }
}
