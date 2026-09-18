#include "../types.h"
#include "model_effect_coefficients.h"
#include "model_effect_wrapped_value.h"
#include "trig_constants.h"

s32 func_8005A618(s32 index)
{
    s32 offset = index * MODEL_SLOT_SIZE;
    ModelEffectCoefficient *coefficient;
    s32 value;
    ModelSlotCF8TailView *record;
    s32 biased;

    record = (ModelSlotCF8TailView *)(offset + (s32)&D_800F3938);
    /* Retail rebuilds the alias base for the selector-byte load. */
    coefficient = func_8005F1A4(
        record->prefix.bytes.field_0A[
            (((u8 *)&D_800F3938) + offset)[MODEL_SLOT_CF8_DFE_OFFSET]
        ] & 0x1F
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
