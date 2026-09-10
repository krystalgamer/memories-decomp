#include "../types.h"
#include "duel_grid.h"
#include "display_object.h"
#include "display_parent_links.h"
#include "func_80022EEC.h"

void func_80022F98(DisplayParent *parent, volatile DisplayObject *object)
{
    DisplayObject *base;
    int index;

    if (object != 0) {
        base = parent->position_base;
        object->position.h.field_28 = object->field_30.h.field_30 - base->field_30.h.field_30;
        object->position.h.field_2A = object->field_30.h.field_32 - base->field_30.h.field_32;
        index = parent->index;
        object->field_6C = 1;
        object->update = (DisplayObjectCallback)func_80022EEC;
        object->field_2C.h.field_2C = index;
    }
}

