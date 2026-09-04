#include "../types.h"

typedef struct {
    u8 pad_00[8];
    u16 flags;
    u8 pad_0A[0x5D];
    u8 field_67;
    u8 field_68;
    u8 field_69;
} DisplayObjectConfig;

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
