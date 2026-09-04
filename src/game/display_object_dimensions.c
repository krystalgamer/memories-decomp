#include "../types.h"

typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x0E];
    s16 half_height;
    s16 half_width;
    u8 pad_1C[0x14];
    s16 x;
    s16 y;
    u8 pad_34[0x08];
    s16 height;
    s16 width;
    s16 field_40;
    s16 field_42;
    u8 pad_44[0x04];
    s16 half_height_2;
    s16 half_width_2;
    u8 pad_4C[0x10];
    u8 field_5C;
    u8 field_5D;
    u8 pad_5E[0x08];
    u8 field_66;
} DisplayObject;

DisplayObject *func_80040510(
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

    object->height = height;
    half_height = height / 2;
    object->x = x;
    object->y = y;
    object->half_height = half_height;
    object->half_height_2 = half_height;

    object->flags |= 8;

    object->width = width;
    half_width = width / 2;
    object->field_5C = field_5C;
    object->field_5D = field_5D;
    object->field_66 = field_66;
    object->field_40 = field_40;
    object->field_42 = field_42;
    object->half_width = half_width;
    object->half_width_2 = half_width;
    return object;
}
