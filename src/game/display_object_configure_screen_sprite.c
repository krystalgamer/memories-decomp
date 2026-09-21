#include "../types.h"
#include "display_object.h"
#include "display_object_config.h"
#include "display_object_layout.h"

DisplayObject *DisplayObject_ConfigureScreenSprite(
    DisplayObject *object,
    s32 x,
    s32 y,
    s32 height,
    s32 width,
    s32 field_5C,
    s32 field_5D,
    s32 field_66,
    s32 field_40,
    s32 field_42)
{
    s32 half_height;
    s32 half_width;

    object->field_3C.h.field_3C = height;
    half_height = height / 2;
    object->field_30.h.field_30 = x;
    object->field_30.h.field_32 = y;
    object->field_18 = half_height;
    object->field_48.h.field_48 = half_height;

    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;

    object->field_3C.h.field_3E = width;
    half_width = width / 2;
    *(u8 *)&object->field_5C = field_5C;
    ((u8 *)&object->field_5C)[1] = field_5D;
    object->field_66 = field_66;
    object->field_40.h.field_40 = field_40;
    object->field_40.h.field_42 = field_42;
    object->field_1A = half_width;
    object->field_48.h.field_4A = half_width;
    return object;
}
