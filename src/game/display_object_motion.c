#include "../types.h"
#include "display_object_motion.h"
#include "display_object_interpolation.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "trig_constants.h"

void func_8001EC70(DisplayObject *object)
{
    if (!func_80042B98((DisplayObjectLifecycle *)object)) {
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
        object->field_60 = 0;
        object->field_2C.h.field_2E = 0;
    }
    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, (s16)object->position.h.field_28,
                 (s16)object->position.h.field_2A, object->field_60);
    object->field_60 +=
        TRIG_ANGLE_HALF_TURN / (s16)object->field_2C.h.field_2C;
    if (object->field_60 >= TRIG_ANGLE_HALF_TURN) {
        object->field_6C = 0;
        object->update = 0;
    }
}

void func_8001ED20(DisplayObject *object)
{
    if (!func_80042B98((DisplayObjectLifecycle *)object)) {
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
        object->field_60 = 0;
        object->field_2C.h.field_2E = 0;
    }
    if (object->field_20.b.field_22) {
        object->field_20.b.field_22 += 0x40 / (s16)object->field_2C.h.field_2C;
    }
    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, (s16)object->position.h.field_28,
                 (s16)object->position.h.field_2A, object->field_60);
    object->field_60 +=
        TRIG_ANGLE_HALF_TURN / (s16)object->field_2C.h.field_2C;
    if (object->field_60 >= TRIG_ANGLE_HALF_TURN) {
        object->field_30.word = object->position.word;
        object->field_20.b.field_22 = 0;
        if (!object->field_20.b.field_21) {
            object->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
        object->field_6C = 0;
        object->update = 0;
    }
}
