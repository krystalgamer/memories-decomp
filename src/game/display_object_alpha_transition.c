#include "../types.h"
#include "display_object_update_alpha_transition.h"
#include "display_object_start_alpha_transition.h"

/* A display object's alpha transition: DisplayObject_StartAlphaTransition
   seeds the start level and the target/step pair and installs
   DisplayObject_UpdateAlphaTransition as the object's update callback, which
   steps byte 0x21 until it passes the target and then uninstalls itself. */

void DisplayObject_UpdateAlphaTransition(DisplayObject *object) {
    s32 n = object->field_20.b.rotation_y +
        *(u8 *)&object->position.h.field_2A;
    s32 d;

    object->field_20.b.rotation_y = n;
    if (*(s16 *)&object->position.h.field_2A >= 0) {
        d = *(u8 *)&object->position.h.field_28 - n;
    } else {
        d = n - *(u8 *)&object->position.h.field_28;
    }

    if ((s8)d < 0) {
        u8 c = *(u8 *)&object->position.h.field_28;

        object->field_6C = 0;
        object->update = 0;
        object->field_20.b.rotation_y = c;
        if (c == 0) {
            object->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
    }
}

void DisplayObject_StartAlphaTransition(DisplayObject *object, u8 start, s16 target, s16 step)
{
    object->field_6C = 1;
    object->field_20.b.rotation_y = start;
    *(s16 *)&object->position.h.field_28 = target;
    *(s16 *)&object->position.h.field_2A = step;
    object->update = (DisplayObjectCallback)DisplayObject_UpdateAlphaTransition;
    object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
}
