#include "../types.h"
#include "display_object_config.h"

void DisplayObject_SetResourceVariant(DisplayObjectConfig *object, s32 value)
{
    object->field_69 = value;
    object->flags &= 0xFFEF;
}
