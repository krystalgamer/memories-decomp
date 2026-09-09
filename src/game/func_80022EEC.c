#include "../types.h"
#include "duel_side_state.h"
#include "duel_selection_layout.h"
#include "func_80022EEC.h"
#include "display_object_api.h"

void func_80022EEC(DisplayObject *object)
{
    u8 *base = D_800E9F10;
    s32 offset =
        *(s16 *)&object->field_2C.h.field_2C * DUEL_SELECTION_RECORD_SIZE;
    DisplayObject *parent;

    offset += D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
    parent = *(DisplayObject **)(base + offset);
    if (parent == 0) {
        func_8004036C(object);
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
