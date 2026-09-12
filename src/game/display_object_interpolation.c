#include "../types.h"
#include "../psyq/libgte.h"
#include "trig_constants.h"
#include "display_object_interpolation.h"

void DisplayObject_SavePosition(DisplayObjectSnapshot *object)
{
    object->field_36 = object->field_30;
    object->field_38 = object->field_32;
}

void DisplayObject_InterpolatePositionCosine(
    DisplayObjectPosition *object, s32 arg1, s32 arg2, s32 arg3
)
{
    s32 x = (object->x + arg1) >> 1;
    s32 y = (object->y + arg2) >> 1;
    s32 scale = rcos(arg3);
    s32 scale2 = scale;

    object->out_x = x + (object->x - x) * scale / ONE;
    object->out_y = y + (object->y - y) * scale2 / ONE;
}

void Widget_SlideSine(
    DisplayObjectPosition *object,
    int target_x,
    int target_y,
    int phase
)
{
    int dx = object->x - target_x;
    int dy = object->y - target_y;
    int factor;

    if (phase < 0) {
        factor = rsin(phase + TRIG_ANGLE_QUARTER_TURN);
        object->out_x = object->x - (dx * factor) / ONE;
        object->out_y = object->y - (dy * factor) / ONE;
    } else {
        factor = -rsin(phase);
        object->out_x = target_x - (dx * factor) / ONE;
        object->out_y = target_y - (dy * factor) / ONE;
    }
}
