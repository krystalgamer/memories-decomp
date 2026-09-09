#include "../types.h"
#include "func_80019B2C.h"

void func_80019B2C(DisplayObject *arg0) {
    s32 n = arg0->field_20.b.field_21 +
        *(u8 *)&arg0->position.h.field_2A;
    s32 d;

    arg0->field_20.b.field_21 = n;
    if (*(s16 *)&arg0->position.h.field_2A >= 0) {
        d = *(u8 *)&arg0->position.h.field_28 - n;
    } else {
        d = n - *(u8 *)&arg0->position.h.field_28;
    }

    if ((s8)d < 0) {
        u8 c = *(u8 *)&arg0->position.h.field_28;

        arg0->field_6C = 0;
        arg0->update = 0;
        arg0->field_20.b.field_21 = c;
        if (c == 0) {
            arg0->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
    }
}
