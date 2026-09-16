#include "../types.h"
#include "func_80019B2C.h"

void DisplayObject_UpdateAlphaTransition(DisplayObject *object) {
    s32 n = object->field_20.b.field_21 +
        *(u8 *)&object->position.h.field_2A;
    s32 d;

    object->field_20.b.field_21 = n;
    if (*(s16 *)&object->position.h.field_2A >= 0) {
        d = *(u8 *)&object->position.h.field_28 - n;
    } else {
        d = n - *(u8 *)&object->position.h.field_28;
    }

    if ((s8)d < 0) {
        u8 c = *(u8 *)&object->position.h.field_28;

        object->field_6C = 0;
        object->update = 0;
        object->field_20.b.field_21 = c;
        if (c == 0) {
            object->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
    }
}
