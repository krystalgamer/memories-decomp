#include "../types.h"
#include "display_object_config.h"

void DisplayObject_UpdateResourceVariant(
    DisplayObjectConfig *object,
    s32 value
)
{
    if (object->field_69 != value) {
        object->field_69 = value;
        object->flags &= 0xFFEF;
    }
}
