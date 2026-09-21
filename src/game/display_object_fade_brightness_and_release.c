#include "../types.h"
#include "display_object_core.h"
#include "display_object_lifecycle.h"

void DisplayObject_FadeBrightnessAndRelease(DisplayObjectLifecycle *object)
{
    s32 value = object->red - object->fade_step;

    if (value > 0) {
        object->blue = value;
        object->green = value;
        object->red = value;
    } else {
        DisplayObject_ReleaseIfPresent(object);
    }
}
