#include "../types.h"
#include "display_object_layout.h"

void func_800427DC(u8 *object, int value)
{
    u16 flags = *(u16 *)(object + 8);

    *(u32 *)(object + 0x54) = 0;
    *(u32 *)(object + 0x4C) = 0;
    *(u32 *)(object + 0x44) = 0;
    *(u32 *)(object + 0x3C) = 0;
    *(u32 *)(object + 0x34) = 0;
    *(u32 *)(object + 0x2C) = 0;
    *(u32 *)(object + 0x10) = 0;
    object[0x21] = 0;
    object[0x20] = 0;
    object[0x22] = 0;
    *(u16 *)(object + 0x1C) = 0;
    *(u16 *)(object + 0x1A) = 0;
    *(u16 *)(object + 0x18) = 0;
    object[0x5A] = value;
    *(u16 *)(object + 8) = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void func_80042824(u8 *object, int value)
{
    u32 initial = 0x00808080;
    u16 flags = *(u16 *)(object + 8);

    *(u32 *)(object + 0x68) = initial;
    *(u32 *)(object + 0x5C) = initial;
    *(u32 *)(object + 0x50) = initial;
    *(u32 *)(object + 0x44) = initial;
    *(u32 *)(object + 0x38) = initial;
    *(u32 *)(object + 0x2C) = initial;
    *(u32 *)(object + 0x10) = 0;
    object[0x21] = 0;
    object[0x20] = 0;
    object[0x22] = 0;
    *(u16 *)(object + 0x1C) = 0;
    *(u16 *)(object + 0x1A) = 0;
    *(u16 *)(object + 0x18) = 0;
    object[0x72] = value;
    *(u16 *)(object + 8) = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

typedef struct {
    u8 pad_00[0x54];
    void *resource;
} DisplayObjectResource;

extern void func_80040468(
    DisplayObjectResource *,
    s32,
    s32,
    s32,
    s32,
    s32
);
extern void func_800404CC(
    void *,
    s32,
    s32,
    s32,
    s32,
    s32,
    s32,
    s32
);

void func_80042874(
    DisplayObjectResource *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    void *resource
)
{
    object->resource = resource;
    func_80040468(object, arg1, arg2, arg3, arg4, arg5);
}

void func_800428A8(
    void *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    s32 arg6,
    s32 arg7,
    s32 arg8
)
{
    *(s32 *)((u8 *)object + 0x54) = arg8;
    func_800404CC(object, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

typedef struct {
    u8 pad_00[0x14];
    s16 field_14;
    s8 field_16;
    u8 field_17;
} DisplayObjectAnimationState;

extern volatile u16 D_8009AF74[4] __attribute__((section(".sdata")));
extern u16 D_8009AF76;
extern u16 D_8009AF7A;

s32 func_800428EC(u8 *object, s8 value)
{
    u32 index = object[0x17];
    volatile u16 *table = D_8009AF74;
    s32 result;

    object[0x16] = value;
    result = table[index] - value;
    *(u16 *)(object + 0x14) = result;
    return result;
}

void func_80042918(DisplayObjectAnimationState *object)
{
    object->field_17 = 1;
    object->field_14 = D_8009AF76 - object->field_16;
}

void func_8004293C(DisplayObjectAnimationState *object)
{
    object->field_17 = 3;
    object->field_14 = D_8009AF7A - object->field_16;
}

int func_80042960(char *object)
{
    void (*callback)(void) = *(void (**)(void))(object + 0x24);

    if (callback != 0)
        callback();
    return ((*(u16 *)(object + 8) & DISPLAY_OBJECT_RENDERABLE_MASK) ==
            DISPLAY_OBJECT_RENDERABLE_MASK);
}

typedef struct {
    u8 pad_00[0x54];
    u8 *base;
} DisplayObjectStream;

typedef struct {
    u8 pad_00[0x30];
    s16 x;
    s16 y;
    s16 z;
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    u8 pad_3C[0x26];
    u8 fraction_x;
    u8 fraction_y;
    u8 fraction_z;
} DisplayObjectVelocity;

u32 func_800429A8(const u8 *data)
{
    return (data[1] << 8) | data[0];
}

u8 *func_800429BC(DisplayObjectStream *object, const u8 *data)
{
    return object->base + ((data[1] << 8) | data[0]);
}

void func_800429D8(DisplayObjectVelocity *object)
{
    if (object != 0) {
        object->velocity_x = 0;
        object->velocity_y = 0;
        object->velocity_z = 0;
        object->fraction_x = 0x80;
        object->fraction_y = 0x80;
        object->fraction_z = 0x80;
    }
}

void func_80042A00(DisplayObjectVelocity *object)
{
    int value = (object->x << 8) | object->fraction_x;

    value += object->velocity_x;
    object->fraction_x = value;
    object->x = value >> 8;
}

void func_80042A28(DisplayObjectVelocity *object)
{
    int value = (object->y << 8) | object->fraction_y;

    value += object->velocity_y;
    object->fraction_y = value;
    object->y = value >> 8;
}

void func_80042A50(DisplayObjectVelocity *object)
{
    int value = (object->z << 8) | object->fraction_z;

    value += object->velocity_z;
    object->fraction_z = value;
    object->z = value >> 8;
}

void func_80042A78(DisplayObjectVelocity *object)
{
    func_80042A00(object);
    func_80042A28(object);
}

void func_80042AA4(DisplayObjectVelocity *object)
{
    func_80042A00(object);
    func_80042A28(object);
    func_80042A50(object);
}

s32 func_80042AD8(s32 value, s32 target, s32 step)
{
    if (target < 0) {
        value -= step;
        if (value < target) {
            value = target;
        }
    } else {
        value += step;
        if (value > target) {
            value = target;
        }
    }
    return value;
}

s32 func_80042B08(s32 value, s32 step)
{
    if (value < 0) {
        value += step;
        if (value > 0) {
            value = 0;
        }
    } else {
        value -= step;
        if (value < 0) {
            value = 0;
        }
    }
    return value;
}
