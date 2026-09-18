#include "../types.h"
#include "func_80019B2C.h"
#include "func_80019BA0.h"

/* A display object's alpha transition: func_80019BA0 seeds the start level
   and the target/step pair and installs DisplayObject_UpdateAlphaTransition
   as the object's update callback, which steps byte 0x21 until it passes the
   target and then uninstalls itself. */

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

void func_80019BA0(DisplayObject *arg0, u8 arg1, s16 arg2, s16 arg3)
{
    arg0->field_6C = 1;
    arg0->field_20.b.field_21 = arg1;
    *(s16 *)&arg0->position.h.field_28 = arg2;
    *(s16 *)&arg0->position.h.field_2A = arg3;
    arg0->update = (DisplayObjectCallback)DisplayObject_UpdateAlphaTransition;
    arg0->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
}
