#include "../types.h"
#include "display_object_config.h"
#include "display_object_helpers.h"

void DisplayObject_InitializeTexturedGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
)
{
    u32 initial = 0x00808080;
    u16 flags = object->flags;

    *(u32 *)&object->field_68 = initial;
    *(u32 *)&object->field_5C = initial;
    object->field_50.word = initial;
    object->field_44.word = initial;
    object->field_38.word = initial;
    object->field_2C.word = initial;
    object->field_10 = 0;
    object->field_20.b.field_21 = 0;
    object->field_20.b.field_20 = 0;
    object->field_20.b.field_22 = 0;
    object->field_1C = 0;
    object->field_1A = 0;
    object->field_18 = 0;
    ((u8 *)object)[0x72] = has_secondary_quad;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void DisplayObject_ConfigureSpriteWithResource(
    DisplayObject *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    void *resource
)
{
    object->field_54 = resource;
    DisplayObject_ConfigureSpriteResource(object, arg1, arg2, arg3, arg4, arg5);
}

void DisplayObject_ConfigureSpriteAtPositionWithResource(
    DisplayObject *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    s32 arg6,
    s32 arg7,
    void *resource
)
{
    object->field_54 = resource;
    DisplayObject_ConfigureSpriteAtPosition(object, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

s32 DisplayObject_SetDepthOffset(DisplayObject *obj, s8 value)
{
    u32 index = obj->ot_index;
    volatile u16 *table = D_8009AF74;
    s32 result;

    obj->field_16 = value;
    result = table[index] - value;
    obj->field_14 = result;
    return result;
}

void DisplayObject_SelectOrderingTable1(DisplayObject *object)
{
    object->ot_index = 1;
    object->field_14 = D_8009AF74[1] - object->field_16;
}

void DisplayObject_SelectOrderingTable3(DisplayObject *object)
{
    object->ot_index = 3;
    object->field_14 = D_8009AF74[3] - object->field_16;
}
