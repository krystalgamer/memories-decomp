#include "../types.h"
#include "display_object.h"
#include "func_8001B780.h"

void func_8001B780(DuelHandStackState *object)
{
    DisplayObject *inner = object->position_object;

    *(s16 *)&inner->field_30.h.field_30 = object->slot_index * 60 + 14;
    *(s16 *)&inner->field_30.h.field_32 = 194;
}
