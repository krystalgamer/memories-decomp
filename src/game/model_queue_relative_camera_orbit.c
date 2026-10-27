#include "../types.h"
#include "model_update_view_metrics.h"
#include "model_copy_slot_position.h"
#include "../psyq/memory.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_queue_relative_camera_orbit.h"
#include "model_effect_state.h"

void Model_QueueRelativeCameraOrbit(
    s32 slot_selector,
    s32 distance,
    s32 yaw,
    s32 pitch,
    SVECTOR *target_offset,
    s32 key_magnitude
) {
    u16 target_halfwords[4];
    u16 orbit_halfwords[4];
    ModelViewMetrics *view_metrics;
    s32 one;
    s32 offset_sign;
    s32 biased_yaw;
    s32 biased_pitch;
    s32 yaw_word;
    s32 x;
    s32 queue_mode;

    view_metrics = Model_GetViewMetricsBuffer();
    memset(target_halfwords, 0, sizeof(target_halfwords));
    target_halfwords[3] = slot_selector | 0x80;
    one = 1;

    if (gModel_bCameraHoldState == one && gModel_bHandlerCallbackState == one) {
        return;
    }

    if (slot_selector == 2) {
        Model_CopySlotPosition(2, (u16 *)target_halfwords);
        target_halfwords[3] = one;
    }

    if (target_offset != 0) {
        offset_sign = -1;
        if (slot_selector <= 0) {
            offset_sign = 1;
        }
        target_halfwords[0] = target_halfwords[0] + target_offset->vx * offset_sign;
        /* Retail zero-extends only the middle component. */
        target_halfwords[1] = target_halfwords[1] + *(u16 *)&target_offset->vy;
        target_halfwords[2] = target_halfwords[2] + target_offset->vz * offset_sign;
    }

    orbit_halfwords[0] = distance;
    x = (s16)view_metrics->yaw;
    biased_yaw = yaw + MODEL_ANGLE_FULL_TURN;
    if (slot_selector <= 0) {
        x = x - 0xC00;
    } else {
        x = x - 0x400;
    }
    orbit_halfwords[1] = (biased_yaw - x) - (biased_yaw - x) / MODEL_ANGLE_FULL_TURN *
        MODEL_ANGLE_FULL_TURN;
    yaw_word = orbit_halfwords[1];
    if (*(s16 *)&orbit_halfwords[1] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        orbit_halfwords[1] = yaw_word - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&orbit_halfwords[1] < -MODEL_ANGLE_HALF_TURN) {
        orbit_halfwords[1] = orbit_halfwords[1] + MODEL_ANGLE_FULL_TURN;
    }

    biased_pitch = pitch + MODEL_ANGLE_FULL_TURN;
    orbit_halfwords[2] = (biased_pitch - biased_pitch / MODEL_ANGLE_FULL_TURN * MODEL_ANGLE_FULL_TURN) -
        view_metrics->pitch;
    if (*(s16 *)&orbit_halfwords[2] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        orbit_halfwords[2] = orbit_halfwords[2] - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&orbit_halfwords[2] < -MODEL_ANGLE_HALF_TURN) {
        orbit_halfwords[2] = orbit_halfwords[2] + MODEL_ANGLE_FULL_TURN;
    }

    queue_mode = *(s8 *)&gModel_nEffectKeyBatchOrdinal;
    x = queue_mode;
    orbit_halfwords[3] = 4;
    if (x < 0) {
        queue_mode = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = gModel_nEffectKeyBatchOrdinal + 1;
        queue_mode = x > 0;
    }
    Model_ApplyEffectKeyQueueCommand(
        queue_mode, (ModelEffectEndpoint *)orbit_halfwords, (ModelEffectEndpoint *)target_halfwords, key_magnitude
    );
}
