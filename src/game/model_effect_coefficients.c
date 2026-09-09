#include "../types.h"
#include "../ygo_types.h"
#include "model_effect_coefficients.h"
#include "model_slot_support.h"

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

    func_80059000(level, (s16 *)&local);

    if (local.max < 50) {
        local.max = 50;
    }

    local.max -= 300;
    delta = local.max;

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
