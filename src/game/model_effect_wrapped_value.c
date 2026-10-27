#include "../types.h"
#include "model_effect_coefficients.h"
#include "model_effect_wrapped_value.h"
#include "trig_constants.h"

s32 Model_GetSlotCameraPresetYaw(s32 slot_index)
{
    s32 slot_offset = slot_index * MODEL_SLOT_SIZE;
    ModelCameraOrbitPreset *preset;
    s32 yaw;
    ModelSlotCF8TailView *slot_metadata;
    s32 biased_yaw;

    slot_metadata = (ModelSlotCF8TailView *)(slot_offset + (s32)&gModel_Slot0MetadataView);
    /* Retail rebuilds the alias base for the selector-byte load. */
    preset = ModelEffect_GetCoefficient(
        slot_metadata->prefix.bytes.guardian_star_options[
            (((u8 *)&gModel_Slot0MetadataView) + slot_offset)[MODEL_SLOT_CF8_DFE_OFFSET]
        ] & 0x1F
    );

    yaw = preset->yaw;
    if (slot_index <= 0) {
        biased_yaw = yaw + TRIG_ANGLE_FULL_TURN +
                 3 * TRIG_ANGLE_QUARTER_TURN;
        return biased_yaw % TRIG_ANGLE_FULL_TURN;
    }

    biased_yaw = yaw + TRIG_ANGLE_FULL_TURN + TRIG_ANGLE_QUARTER_TURN;
    return biased_yaw % TRIG_ANGLE_FULL_TURN;
}
