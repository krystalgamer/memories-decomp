#include "../types.h"
#include "display_object_helpers.h"

void DisplayObject_StepPositionXY(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
}

void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
    DisplayObject_StepPositionZ(object);
}

s32 DisplayObject_StepToward(s32 value, s32 target, s32 step)
{
    if (target < 0) {
        value -= step;
        if (value < target) {
            value = target;
        }
    } else {
        value += step;
        if (value > target) {
            value = target;
        }
    }
    return value;
}
