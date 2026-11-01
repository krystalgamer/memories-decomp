/*
 * Evaluates the two camera keyframe channels, then resolves relative orbits.
 * Timing sources are live after callbacks, but the first timing destination
 * and the output view are captured. The slot copier writes four halfwords;
 * slot_position and follow_target_position retain that complete extent.
 *
 * All 3,172 bytes match with the existing uniform G8 split/no-strength-reduce
 * profile. The ordinary profile creates an extra pose-component induction
 * pointer and spills the evaluated-buffer pointer. The middle-axis helper
 * retains the signed adjustment's publication boundary; its arithmetic is
 * word-sized before the result is narrowed.
 *
 * Orbit projection uses the separate horizontal/vertical components already
 * used by Model_StepViewOrbit. The early elapsed_time local, remaining-duration capture
 * and opposite-view pointer cursor retain the original register lifetimes.
 * Finite valid key programs require nonzero interpolation/noise divisors.
 */
#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800F5768-0x800F5918, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 D_800F5768[0x1B0];

#include "camera_view.h"
#include "model_keyframe_spline.h"
#include "model_transfer_flags.h"
#include "model_effect_state.h"
#include "model_get_slot_readiness.h"
#include "model_get_frame_step.h"
#include "model_copy_slot_position.h"
#include "model_update_view_metrics.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "model_step_view_orbit.h"
#include "../psyq/rand.h"

#define LIMIT(v, n) ((v) > 0 ? ((v) < (n) ? (v) : (n)) : \
    ((v) > -(n) ? (v) : -(n)))
#define KEY_VIEW(key) ((Key *)(key))

static inline s32 Keyframe_ApplyDeadzone(s16 *axis, s32 amount)
{
    s32 value = *axis;
    if (value > 0) {
        s32 result = value - amount;
        return result < 0 ? 0 : result;
    } else {
        s32 result = value + amount;
        return result > 0 ? 0 : result;
    }
}

void Model_UpdateEffectKeyframes(void)
{
    s32 *output_channels[2];
    s16 slot_position[4];
    s16 spline_result[4];
    s16 follow_target_position[4];
    s16 follow_delta[4];
    s32 active_channel_count;
    s32 persistent_channel_count;
    s32 channel_index;
    s32 channel_byte_offset;
    s32 **output_cursor;
    s16 *pose_halfwords;
    s16 *spline_result_cursor;

    output_channels[0] = (s32 *)Model_GetCameraViewBuffer();
    output_channels[1] = output_channels[0] + 3;
    persistent_channel_count = 0;
    active_channel_count = 0;
    if (!gModel_bEffectKeyCount || !gModel_pCurrentEffectKey) {
        return;
    }
    if (gModel_bEffectKeyPreparationPending && (gModel_bCameraHoldState != 1 || gModel_bHandlerCallbackState != 1)) {
        Key *preparation_key = gModel_aEffectKeys;
        s32 preparation_index;
        for (preparation_index = 0; preparation_index < gModel_bEffectKeyCount; preparation_index++) {
            Model_PrepareEffectKey(preparation_key++);
        }
        gModel_bEffectKeyPreparationPending = 0;
        gModel_nEffectKeyBatchOrdinal = -1;
    }
    if (gModel_pCurrentEffectKey->duration != 0x4000) {
        Key *progress_write_key = gModel_pCurrentEffectKey;
        s32 frame_step = Model_GetFrameStep();
        s32 elapsed_time;
        if (gModel_pCurrentEffectKey->progress + frame_step <
            gModel_pCurrentEffectKey->duration) {
            frame_step = Model_GetFrameStep();
            elapsed_time = gModel_pCurrentEffectKey->progress + frame_step;
        } else {
            elapsed_time = gModel_pCurrentEffectKey->duration;
        }
        progress_write_key->progress = elapsed_time;
    }
    channel_index = 0;
    spline_result_cursor = spline_result;
    output_cursor = output_channels;
    pose_halfwords = (s16 *)gModel_aPoseSnapshot;
    channel_byte_offset = 0;
    do {
        u8 *channel_key_bytes = (u8 *)gModel_pCurrentEffectKey;
        s16 *channel_halfwords = (s16 *)(channel_key_bytes + channel_byte_offset);
        if (channel_halfwords[3] == 0) {
            goto next_channel;
        }
        switch (channel_halfwords[3]) {
        case 0x80:
        case 0x81: {
            s32 slot_index = (s16)((u16)channel_halfwords[3] & 0xFF7F);
            s32 elapsed_time = KEY_VIEW(channel_key_bytes)->progress;
            s32 duration_time = KEY_VIEW(channel_key_bytes)->duration;
            s32 remaining_time;
            if (Model_GetSlotReadiness(slot_index) != 1) {
                break;
            }
            Model_CopySlotPosition(slot_index, (u16 *)slot_position);
            {
                s16 *slot_offset_halfwords = (s16 *)((u8 *)gModel_pCurrentEffectKey + channel_byte_offset);
                slot_position[0] += (u16)slot_offset_halfwords[0];
                slot_position[1] += (u16)slot_offset_halfwords[1];
                slot_position[2] += (u16)slot_offset_halfwords[2];
            }
            remaining_time = duration_time - elapsed_time;
            (*output_cursor)[0] = pose_halfwords[0] * remaining_time / duration_time
                + slot_position[0] * elapsed_time / duration_time;
            (*output_cursor)[1] = pose_halfwords[1] * remaining_time / duration_time
                + slot_position[1] * elapsed_time / duration_time;
            (*output_cursor)[2] = pose_halfwords[2] * remaining_time / duration_time
                + slot_position[2] * elapsed_time / duration_time;
            break;
        }
        case 1: {
            s32 axis_index;
            s16 *spline_component_cursor;
            s32 **spline_output_cursor;
            Model_EvaluateKeyframeSpline(KEY_VIEW(channel_key_bytes), channel_index,
                KEY_VIEW(channel_key_bytes)->progress, 0, spline_result_cursor);
            axis_index = 0;
            spline_output_cursor = output_cursor;
            spline_component_cursor = spline_result_cursor;
            for (; axis_index < 3; axis_index++) {
                (*spline_output_cursor)[axis_index] = *spline_component_cursor++;
            }
            break;
        }
        case 2:
        case 3: {
            s32 slot_index;
            s32 follow_amount;
            persistent_channel_count++;
            slot_index = channel_halfwords[0];
            follow_amount = channel_halfwords[1];
            if (Model_GetSlotReadiness(slot_index) != 1) {
                break;
            }
            Model_CopySlotPosition(slot_index, (u16 *)follow_target_position);
            {
                s16 *follow_reference_halfwords = (s16 *)((u8 *)gModel_pCurrentEffectKey + channel_byte_offset);
                follow_delta[0] = follow_target_position[0] - (u16)follow_reference_halfwords[8];
                follow_delta[1] = follow_target_position[1] - (u16)follow_reference_halfwords[9];
                follow_delta[2] = follow_target_position[2] - (u16)follow_reference_halfwords[10];
                if (follow_reference_halfwords[3] == 2) {
                    if (follow_amount == 0) {
                        follow_amount = 10;
                    }
                    {
                        s32 adjusted_delta = follow_delta[0];
                        if (adjusted_delta > 0) {
                            adjusted_delta -= follow_amount;
                            if (adjusted_delta < 0) {
                                adjusted_delta = 0;
                            }
                        } else {
                            adjusted_delta += follow_amount;
                            if (adjusted_delta > 0) {
                                adjusted_delta = 0;
                            }
                        }
                        follow_delta[0] = adjusted_delta;
                    }
                    {

                        follow_delta[1] = Keyframe_ApplyDeadzone(&follow_delta[1], follow_amount);
                    }
                    {
                        s32 adjusted_delta = follow_delta[2];
                        if (adjusted_delta > 0) {
                            adjusted_delta -= follow_amount;
                            if (adjusted_delta < 0) {
                                adjusted_delta = 0;
                            }
                        } else {
                            adjusted_delta += follow_amount;
                            if (adjusted_delta > 0) {
                                adjusted_delta = 0;
                            }
                        }
                        follow_delta[2] = adjusted_delta;
                    }
                    follow_delta[0] = LIMIT(follow_delta[0], 60);
                    follow_delta[1] = LIMIT(follow_delta[1], 60);
                    follow_delta[2] = LIMIT(follow_delta[2], 60);
                    (*output_cursor)[0] += follow_delta[0];
                    (*output_cursor)[1] += follow_delta[1];
                    (*output_cursor)[2] += follow_delta[2];
                    {
                        s16 *committed_reference_halfwords =
                            (s16 *)((u8 *)gModel_pCurrentEffectKey + channel_byte_offset);
                        committed_reference_halfwords[8] += (u16)follow_delta[0];
                        committed_reference_halfwords[9] += (u16)follow_delta[1];
                        committed_reference_halfwords[10] += (u16)follow_delta[2];
                    }
                } else {
                    if (follow_amount == 0) {
                        follow_amount = 2000;
                    }
                    follow_delta[0] = LIMIT(follow_delta[0], follow_amount);
                    follow_delta[1] = LIMIT(follow_delta[1], follow_amount);
                    follow_delta[2] = LIMIT(follow_delta[2], follow_amount);
                    (*output_cursor)[0] = pose_halfwords[0] + follow_delta[0];
                    (*output_cursor)[1] = pose_halfwords[1] + follow_delta[1];
                    (*output_cursor)[2] = pose_halfwords[2] + follow_delta[2];
                }
            }
            break;
        }
        case 5: {
            s32 noise_amplitude = channel_halfwords[0];
            s32 noise_sample_a;
            s32 noise_sample_b;
            persistent_channel_count++;
            if (KEY_VIEW(channel_key_bytes)->progress >= KEY_VIEW(channel_key_bytes)->duration) {
                (*output_cursor)[0] = pose_halfwords[0];
                (*output_cursor)[2] = pose_halfwords[2];
                break;
            }
            noise_sample_a = rand();
            noise_sample_b = rand();
            noise_sample_a >>= 8;
            (*output_cursor)[0] = pose_halfwords[0] + noise_sample_a % noise_amplitude
                - ((noise_sample_b >> 8) % noise_amplitude) / 2;
            noise_sample_a = rand();
            noise_sample_b = rand();
            noise_sample_a >>= 8;
            (*output_cursor)[2] = pose_halfwords[2] + noise_sample_a % noise_amplitude
                - ((noise_sample_b >> 8) % noise_amplitude) / 2;
            break;
        }
        }
        active_channel_count++;
next_channel:
        output_cursor++;
        pose_halfwords += 4;
        channel_index++;
        channel_byte_offset += 8;
    } while (channel_index < 2);
    {
        s16 *orbit_pose_halfwords;
        s32 **output_channel_base;
        channel_index = 0;
        orbit_pose_halfwords = (s16 *)gModel_aPoseSnapshot;
        output_cursor = output_channels;
        output_channel_base = output_cursor;
        do {
            s32 elapsed_time;
            u8 *channel_key_bytes = (u8 *)gModel_pCurrentEffectKey;
            s16 *channel_halfwords = (s16 *)(
                channel_key_bytes + (u32)&((s16 (*)[4])0)[channel_index]
            );
            if (channel_halfwords[3] == 4) {
                s32 duration_time = (elapsed_time = KEY_VIEW(channel_key_bytes)->progress,
                    KEY_VIEW(channel_key_bytes)->duration);
                s32 yaw_step = channel_halfwords[1] * elapsed_time / duration_time;
                s32 pitch_step = channel_halfwords[2] * elapsed_time / duration_time;
                s32 radius = orbit_pose_halfwords[8];
                s32 remaining_time = duration_time - elapsed_time;
                s32 yaw = yaw_step + orbit_pose_halfwords[9];
                s32 pitch = pitch_step + orbit_pose_halfwords[10];
                s32 cosine;
                s32 sine;
                s32 depth;
                s32 horizontal;
                s32 vertical;
                if (channel_halfwords[0] != 0) {
                    radius = (radius * remaining_time + channel_halfwords[0] * elapsed_time) / duration_time;
                }
                cosine = rcos(pitch);
                sine = rsin(pitch);
                horizontal = radius * cosine / 0x1000;
                vertical = radius * sine / 0x1000;
                cosine = rcos(yaw);
                sine = rsin(yaw);
                depth = horizontal * sine / 0x1000;
                horizontal = horizontal * cosine / 0x1000;
                {
                    s32 **opposite_output_cursor = output_channel_base;
                    opposite_output_cursor += channel_index ^ 1;
                    (*output_cursor)[0] = (*opposite_output_cursor)[0] + horizontal;
                    (*output_cursor)[1] = (*opposite_output_cursor)[1] + vertical;
                    (*output_cursor)[2] = (*opposite_output_cursor)[2] + depth;
                }
            }
            channel_index++;
            output_cursor++;
        } while (channel_index < 2);
    }
    if (!active_channel_count && gModel_pCurrentEffectKey->duration == 0x4000) {
        gModel_pCurrentEffectKey = 0;
        gModel_bEffectKeyCount = 0;
        return;
    }
    if (gModel_pCurrentEffectKey->progress >=
        gModel_pCurrentEffectKey->duration) {
        Model_CaptureKeyframePose(0);
        gModel_pCurrentEffectKey++;
        if (gModel_pCurrentEffectKey - gModel_aEffectKeys >= gModel_bEffectKeyCount) {
            if (gModel_bEffectKeyCount >= 2) {
                gModel_pCurrentEffectKey = gModel_aEffectKeys;
                gModel_pCurrentEffectKey->ready = 0;
            } else {
                gModel_pCurrentEffectKey = 0;
                gModel_bEffectKeyCount = 0;
            }
        }
        if (gModel_pCurrentEffectKey) {
            Model_PrepareEffectKey(gModel_pCurrentEffectKey);
            if (gModel_pCurrentEffectKey->magnitude == 0) {
                Model_UpdateEffectKeyframes();
            }
        }
    }
    if (persistent_channel_count == 2) {
        Model_StepViewOrbit(1, 0x1000, 0, 0, 0);
    } else {
        Model_UpdateViewMetrics(0);
    }
}
