#include "../types.h"
#include "display_object_interpolation.h"

void DisplayObject_SavePosition(DisplayObjectSnapshot *object)
{
    object->field_36 = object->field_30;
    object->field_38 = object->field_32;
}
