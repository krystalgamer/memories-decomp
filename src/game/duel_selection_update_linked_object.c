#include "../types.h"
#include "duel_side_state.h"
#include "duel_selection_layout.h"
#include "duel_selection_update_linked_object.h"
#include "display_object_core.h"

void DuelSelection_UpdateLinkedObject(DisplayObject *object)
{
    u8 *base = gDuel_abSelectionSides;
    s32 offset =
        (s16)object->field_2C.h.field_2C * DUEL_SELECTION_RECORD_SIZE;
    DisplayObject *parent;

    offset += gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE;
    parent = ((DuelSelectionRecord *)(base + offset))->field_00;
    if (parent == 0) {
        DisplayObject_ReleaseIfPresent(object);
    } else {
        object->field_30.h.field_30 =
            parent->field_30.h.field_30 + object->position.h.field_28;
        object->field_30.h.field_32 =
            parent->field_30.h.field_32 + object->position.h.field_2A;
        if (parent->field_6C == 0) {
            object->field_6C = 0;
            object->update = 0;
        }
    }
}
