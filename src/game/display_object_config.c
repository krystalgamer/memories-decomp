#include "../types.h"
#include "display_object_layout.h"

extern void DisplayObject_ResetPool(void);

typedef struct {
    u8 pad_00[8];
    u16 flags;
    u8 pad_0A[0x5D];
    u8 field_67;
    u8 field_68;
    u8 field_69;
} DisplayObjectConfig;

void func_800403F0(void)
{
    DisplayObject_ResetPool();
}

void func_80040410(DisplayObjectConfig *object, u8 value)
{
    object->field_69 = value;
    object->flags &= 0xFFEF;
}

void func_80040424(DisplayObjectConfig *object, s32 value)
{
    if (object->field_69 != value) {
        object->field_69 = value;
        object->flags &= 0xFFEF;
    }
}

void func_8004044C(
    DisplayObjectConfig *object,
    u8 field_67,
    u8 field_68,
    u8 field_69
)
{
    object->field_67 = field_67;
    object->field_68 = field_68;
    object->field_69 = field_69;
    object->flags &= 0xFFEF;
}

void *func_80040468(u8 *object, int field_67, int field_68, int field_69,
                    int color, int texture)
{
    u16 flags;

    object[0x67] = field_67;
    object[0x68] = field_68;
    object[0x69] = field_69;
    object[0x66] = color;
    object[0x5E] = color >> 16;
    object[0x5F] = color >> 8;
    *(u16 *)(object + 0x40) = texture & 0x3F0;
    *(u16 *)(object + 0x42) = (texture & 0xF) + 0xF0;
    flags = *(u16 *)(object + 8) & 0xFFDF;
    *(u16 *)(object + 8) = flags;
    if (texture & 0x8000) {
        *(u16 *)(object + 8) = flags | 0x20;
    }
    return object;
}

void func_800404CC(u8 *object, s16 x, s16 y, int field_67, int field_68,
                   int field_69, int color, int texture)
{
    *(s16 *)(object + 0x30) = x;
    *(s16 *)(object + 0x32) = y;
    func_80040468(object, field_67, field_68, field_69, color, texture);
}

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

    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;

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
