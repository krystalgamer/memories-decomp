#include "../types.h"
#include "model_effect_coefficients.h"
#include "trig_constants.h"

s32 func_8005A618(s32 index)
{
    s32 offset = index * MODEL_SLOT_SIZE;
    ModelEffectCoefficient *coefficient;
    s32 value;
    s32 record;
    s32 biased;

    record = offset + (s32)&D_800F3938;
    coefficient = func_8005F1A4(
        *(u8 *)(record + (((u8 *)&D_800F3938) + offset)[0x106] + 0xA) & 0x1F
    );

    value = coefficient->angle;
    if (index <= 0) {
        biased = value + TRIG_ANGLE_FULL_TURN +
                 3 * TRIG_ANGLE_QUARTER_TURN;
        return biased % TRIG_ANGLE_FULL_TURN;
    }

    biased = value + TRIG_ANGLE_FULL_TURN + TRIG_ANGLE_QUARTER_TURN;
    return biased % TRIG_ANGLE_FULL_TURN;
}
