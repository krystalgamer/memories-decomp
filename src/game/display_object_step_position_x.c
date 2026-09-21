#include "../types.h"
#include "display_object_helpers.h"

void DisplayObject_StepPositionX(DisplayObjectVelocity *object)
{
    int value = (object->x << 8) | object->fraction_x;

    value += object->velocity_x;
    object->fraction_x = value;
    object->x = value >> 8;
}
