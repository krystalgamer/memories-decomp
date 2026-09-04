#include "../types.h"

typedef struct {
    s32 first;
    s32 second;
} ModelEffectCoefficient;

typedef struct {
    u8 pad_00[6];
    s16 field_06;
} ModelEffectAdjustment;

extern ModelEffectCoefficient D_80091570[];

extern void func_80059000(s32, ModelEffectAdjustment *);

ModelEffectCoefficient *func_8005F1A4(s32 index)
{
    return &D_80091570[index];
}

s32 func_8005F1B8(s32 level, s32 value)
{
    ModelEffectAdjustment local;
    s16 delta;

    if (level >= 2) {
        return value;
    }

    func_80059000(level, &local);

    if (local.field_06 < 50) {
        local.field_06 = 50;
    }

    local.field_06 -= 300;
    delta = local.field_06;

    if (delta != 0) {
        s32 divisor = 750;
        s32 half = value;

        if (delta > 0) {
            half = (s32)(value + ((u32)value >> 31)) >> 1;
        }
        value += (delta * half) / divisor;
    }

    return value;
}
