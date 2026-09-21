#include "../types.h"
#include "display_object.h"
#include "display_object_config.h"

void DisplayObject_ConfigureSpriteAtPosition(
    void *object,
    s32 x,
    s32 y,
    s32 field_67,
    s32 field_68,
    s32 field_69,
    s32 color,
    s32 texture
)
{
    DisplayObject *o = object;

    o->field_30.h.field_30 = x;
    o->field_30.h.field_32 = y;
    DisplayObject_ConfigureSpriteResource(
        object, field_67, field_68, field_69, color, texture
    );
}
