#include "../types.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "display_object_config.h"

void func_800427DC(DisplayObject *object, int value)
{
    u16 flags = object->flags;

    object->field_54 = 0;
    object->field_4C = 0;
    object->field_44.word = 0;
    object->field_3C.word = 0;
    object->field_34 = 0;
    object->field_2C.word = 0;
    object->field_10 = 0;
    object->field_20.b.field_21 = 0;
    object->field_20.b.field_20 = 0;
    object->field_20.b.field_22 = 0;
    object->field_1C = 0;
    object->field_1A = 0;
    object->field_18 = 0;
    /* Low byte only. field_5A is the s16 countdown func_80041D60
       steps by D_8009B0D8 on this same record; this writes one byte
       of it, and that width is retail's, not a simplification. */
    *(u8 *)&object->field_5A = value;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
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
    func_80040468((u8 *)object, arg1, arg2, arg3, arg4, arg5);
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
    void *resource
)
{
    *(void **)((u8 *)object + 0x54) = resource;
    func_800404CC(object, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

s32 func_800428EC(u8 *object, s8 value)
{
    DisplayObject *obj = (DisplayObject *)object;
    u32 index = obj->ot_index;
    volatile u16 *table = D_8009AF74;
    s32 result;

    obj->field_16 = value;
    result = table[index] - value;
    obj->field_14 = result;
    return result;
}

void func_80042918(DisplayObject *object)
{
    object->ot_index = 1;
    object->field_14 = D_8009AF74[1] - object->field_16;
}

void func_8004293C(DisplayObject *object)
{
    object->ot_index = 3;
    object->field_14 = D_8009AF74[3] - object->field_16;
}

int func_80042960(DisplayObject *object)
{
    /* Read as void (*)(void) and called with no argument on purpose:
       the slot's declared type takes a u8 *, but the ambient argument
       register is what retail passes. See #2887. */
    void (*callback)(void) = *(void (**)(void))((u8 *)object + 0x24);

    if (callback != 0)
        callback();
    return ((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ==
            DISPLAY_OBJECT_RENDERABLE_MASK);
}

u32 func_800429A8(const u8 *data)
{
    return (data[1] << 8) | data[0];
}

u8 *func_800429BC(DisplayObjectStream *object, const u8 *data)
{
    return object->base + ((data[1] << 8) | data[0]);
}

void DisplayObject_ResetVelocity(DisplayObjectVelocity *object)
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

void DisplayObject_StepPositionX(DisplayObjectVelocity *object)
{
    int value = (object->x << 8) | object->fraction_x;

    value += object->velocity_x;
    object->fraction_x = value;
    object->x = value >> 8;
}

void DisplayObject_StepPositionY(DisplayObjectVelocity *object)
{
    int value = (object->y << 8) | object->fraction_y;

    value += object->velocity_y;
    object->fraction_y = value;
    object->y = value >> 8;
}

void DisplayObject_StepPositionZ(DisplayObjectVelocity *object)
{
    int value = (object->z << 8) | object->fraction_z;

    value += object->velocity_z;
    object->fraction_z = value;
    object->z = value >> 8;
}

void DisplayObject_StepPositionXY(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
}

void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
    DisplayObject_StepPositionZ(object);
}

s32 DisplayObject_StepToward(s32 value, s32 target, s32 step)
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

s32 DisplayObject_StepTowardZero(s32 value, s32 step)
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
