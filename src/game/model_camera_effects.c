#include "../types.h"
#include "model_update_view_metrics.h"
#include "model_slot_support.h"
#include "../psyq/memory.h"
#include "model.h"
#include "model_camera_effects.h"
#include "camera_view.h"
#include "model_slot_getters.h"
#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "model_effect_endpoints.h"
#include "model_scene_runtime.h"

/* The model transfer family's .sdata block, 0x8009B074 to 0x8009B07F.
 *
 * model_transfer_flags.h already settled everything this file needs. It
 * records what each byte is, and it worked out that gModel_pCurrentEffectKey is a pointer
 * rather than the s32 two units had spelled it: every retail read uses the
 * word as a base for a load at a fixed offset or a null test. Queue setup
 * stores gModel_aEffectKeys, while the updater advances and wraps that Key cursor
 * or clears it with the count. This file gives the block a definition and
 * includes its owner header so the two cannot drift apart.
 *
 * The header also answers in advance the question a carve would otherwise
 * have to ask, noting that every retail access to the run is gp-relative.
 * That is what .sdata provides, so no consumer needs a declaration arm.
 *
 * This is the last of the three owners in the 40-byte range at 0x8009B058
 * that the notes call the scattered tail. The middle block belongs to the
 * movie pipeline and the first eight bytes to main_menu_load_package_stage.c,
 * which stays extracted.
 *
 * gModel_nEffectKeyBatchOrdinal is initialised to -1. The header describes it as a signed
 * counter armed at -1 and the retail byte is 0xFF, so the semantics recorded
 * there and the image agree; that agreement is also what makes s8 rather
 * than u8 the right spelling for the definition.
 *
 * The block ends with three bytes no symbol names, so splat's label for
 * gModel_bHandlerCallbackState spans four bytes although the object is one -- the same way the
 * label at gFile_ReadyTransferDescriptor ran long. They are written out as padding because an
 * object stopping at gModel_bHandlerCallbackState would leave the window three bytes short.
 *
 * The padding is three separate bytes rather than a u8[3], which is not a
 * style choice. An array of three bytes is given four-byte alignment, so it
 * lands at offset 0x0C instead of 0x09 and the compiler fills 0x09 to 0x0B
 * itself; the section then measures fifteen bytes and the link fails with
 * .initialized_data overlapping the bss image. Separate bytes align to one
 * and pack where they belong. model_graphics_state.c spells its own
 * continuation bytes this way for the same reason.
 *
 * Every value is zero apart from that -1, so the section attributes are load
 * bearing rather than decorative: without them these objects would be placed
 * in .sbss and leave .sdata entirely. */

Key *gModel_pCurrentEffectKey = 0;
u8 gModel_bEffectKeyCount = 0;
u8 gModel_bEffectKeyPreparationPending = 0;
s8 gModel_nEffectKeyBatchOrdinal = -1;
u8 gModel_bCameraHoldState = 0;
u8 gModel_bHandlerCallbackState = 0;
static u8 sModelTransferFlags_Pad7D = 0;
static u8 sModelTransferFlags_Pad7E = 0;
static u8 sModelTransferFlags_Pad7F = 0;

/* Zero-initialized storage this unit owns at 0x800F5768-0x800F5918. The
 * generated model_camera_effects_bss_commons subsegment reserves the range and
 * symbols.txt places each array at its retail address (#2602). */
SVECTOR gModel_aPoseSnapshot[CAMERA_POSE_VECTOR_COUNT + 1];
Key gModel_aEffectKeys[10];

/* Initialized data this unit owns at 0x80091550: four endpoint presets consumed in pairs by
   Model_QueueEffectEndpointPair. */
ModelEffectEndpoint gModel_aEffectEndpoints[MODEL_EFFECT_ENDPOINT_COUNT] = {
    { 0, 0, 0, 2 },
    { 1, 0, 0, 2 },
    { 0, 0, 0, 3 },
    { 1, 0, 0, 3 },
};

/* Initialized data this unit owns at 0x80091570: fifteen camera-orbit preset records in
   the order consumed by ModelEffect_GetCoefficient, Model_SetCameraOrbitFromPreset and Model_QueueCameraOrbitFromPreset. */
ModelCameraOrbitPreset gModel_aCameraOrbitPresets[] = {
    { 700, -512, -256, 0 },
    { 700, 512, -256, 0 },
    { 700, 1536, -256, 0 },
    { 700, 2560, -256, 0 },
    { 700, 0, 0, 0 },
    { 1200, -128, -85, 0 },
    { 1200, 128, -85, 0 },
    { 1200, 1920, -85, 0 },
    { 1200, 2176, -85, 0 },
    { 1800, -256, -256, 0 },
    { 1800, 256, -256, 0 },
    { 1800, 1792, -256, 0 },
    { 1800, 2304, -256, 0 },
    { 700, -512, -512, 0 },
    { 700, 512, -512, 0 },
};

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

/*
 * Evaluates the two camera keyframe channels, then resolves relative orbits.
 * Timing sources are live after callbacks, but the first timing destination
 * and the output view are captured. The slot copier writes four halfwords;
 * slot_position and follow_target_position retain that complete extent.
 *
 * All 3,172 bytes match with the uniform G8 split profile. Both channel
 * loops take every record from the loop counter: the key's endpoint
 * halfwords, the pose snapshot vector and the output view channel are all
 * indexed by channel_index rather than advanced as separate cursors. The
 * middle-axis helper retains the signed adjustment's publication boundary;
 * its arithmetic is word-sized before the result is narrowed.
 *
 * Orbit projection uses the separate horizontal/vertical components already
 * used by Model_StepViewOrbit and reads the opposite channel's output.
 * The early elapsed_time local and remaining-duration capture retain the
 * original register lifetimes.
 * Finite valid key programs require nonzero interpolation/noise divisors.
 */

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
    for (channel_index = 0; channel_index < 2; channel_index++) {
        u8 *channel_key_bytes = (u8 *)gModel_pCurrentEffectKey;
        s16 *channel_halfwords = ((s16 (*)[4])channel_key_bytes)[channel_index];
        if (channel_halfwords[3] != 0) {
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
                    s16 *slot_offset_halfwords = ((s16 (*)[4])gModel_pCurrentEffectKey)[channel_index];
                    slot_position[0] += (u16)slot_offset_halfwords[0];
                    slot_position[1] += (u16)slot_offset_halfwords[1];
                    slot_position[2] += (u16)slot_offset_halfwords[2];
                }
                remaining_time = duration_time - elapsed_time;
                output_channels[channel_index][0] = gModel_aPoseSnapshot[channel_index].vx * remaining_time / duration_time
                    + slot_position[0] * elapsed_time / duration_time;
                output_channels[channel_index][1] = gModel_aPoseSnapshot[channel_index].vy * remaining_time / duration_time
                    + slot_position[1] * elapsed_time / duration_time;
                output_channels[channel_index][2] = gModel_aPoseSnapshot[channel_index].vz * remaining_time / duration_time
                    + slot_position[2] * elapsed_time / duration_time;
                break;
            }
            case 1: {
                s32 axis_index;
                Model_EvaluateKeyframeSpline(KEY_VIEW(channel_key_bytes), channel_index,
                    KEY_VIEW(channel_key_bytes)->progress, 0, spline_result);
                for (axis_index = 0; axis_index < 3; axis_index++) {
                    output_channels[channel_index][axis_index] = spline_result[axis_index];
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
                    s16 *follow_reference_halfwords = ((s16 (*)[4])gModel_pCurrentEffectKey)[channel_index];
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
                        output_channels[channel_index][0] += follow_delta[0];
                        output_channels[channel_index][1] += follow_delta[1];
                        output_channels[channel_index][2] += follow_delta[2];
                        {
                            s16 *committed_reference_halfwords =
                                ((s16 (*)[4])gModel_pCurrentEffectKey)[channel_index];
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
                        output_channels[channel_index][0] = gModel_aPoseSnapshot[channel_index].vx + follow_delta[0];
                        output_channels[channel_index][1] = gModel_aPoseSnapshot[channel_index].vy + follow_delta[1];
                        output_channels[channel_index][2] = gModel_aPoseSnapshot[channel_index].vz + follow_delta[2];
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
                    output_channels[channel_index][0] = gModel_aPoseSnapshot[channel_index].vx;
                    output_channels[channel_index][2] = gModel_aPoseSnapshot[channel_index].vz;
                    break;
                }
                noise_sample_a = rand();
                noise_sample_b = rand();
                noise_sample_a >>= 8;
                output_channels[channel_index][0] = gModel_aPoseSnapshot[channel_index].vx + noise_sample_a % noise_amplitude
                    - ((noise_sample_b >> 8) % noise_amplitude) / 2;
                noise_sample_a = rand();
                noise_sample_b = rand();
                noise_sample_a >>= 8;
                output_channels[channel_index][2] = gModel_aPoseSnapshot[channel_index].vz + noise_sample_a % noise_amplitude
                    - ((noise_sample_b >> 8) % noise_amplitude) / 2;
                break;
            }
            }
            active_channel_count++;
        }
    }
    for (channel_index = 0; channel_index < 2; channel_index++) {
        s32 elapsed_time;
        u8 *channel_key_bytes = (u8 *)gModel_pCurrentEffectKey;
        s16 *channel_halfwords = ((s16 (*)[4])channel_key_bytes)[channel_index];
        if (channel_halfwords[3] == 4) {
            s32 duration_time = (elapsed_time = KEY_VIEW(channel_key_bytes)->progress,
                KEY_VIEW(channel_key_bytes)->duration);
            s32 yaw_step = channel_halfwords[1] * elapsed_time / duration_time;
            s32 pitch_step = channel_halfwords[2] * elapsed_time / duration_time;
            s32 radius = gModel_aPoseSnapshot[2].vx;
            s32 remaining_time = duration_time - elapsed_time;
            s32 yaw = yaw_step + gModel_aPoseSnapshot[2].vy;
            s32 pitch = pitch_step + gModel_aPoseSnapshot[2].vz;
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
            output_channels[channel_index][0] = output_channels[channel_index ^ 1][0] + horizontal;
            output_channels[channel_index][1] = output_channels[channel_index ^ 1][1] + vertical;
            output_channels[channel_index][2] = output_channels[channel_index ^ 1][2] + depth;
        }
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

#define MODEL_EFFECT_ENDPOINT_VIEW(value) ((ModelEffectEndpoint *)(value))
#define KEY_BYTES(key) ((u8 *)(key))

/*
 * Semantic absolute value and keyframe bounds: `Model_PrepareEffectKey`
 *
 * The 1004-byte keyframe routine matches all 251 instruction words under the
 * existing uniform `gcc_2_8_1_g8_split` profile. Its parameter uses the
 * canonical 0x28-byte `Key` layout shared with the evaluator. The duration
 * field at +0x22 is consumed as a denominator by the evaluator below; the
 * retired candidate's "audible radius" description was not the established
 * contract.
 *
 * Two `__builtin_abs` calls are load-bearing. They use the same semantic
 * intrinsic as matching `duel_scene.c` and `SD_ApplyPositionalSound` in `model.c`.
 * The compiler emits its `abs` pseudo-instruction, rather than a hand-written
 * conditional whose scheduled delay slot duplicates a multiply. Both operands
 * come from signed halfwords promoted to s32, including -32768, so their
 * magnitudes fit the result type.
 *
 * Case-local distance temporaries and per-case bound checks preserve the
 * multiply/result registers and let the bound loads fill their original
 * slots. Pose traversal is indexed from `gModel_aPoseSnapshot`, and the path counter is
 * initialized to one before its byte offset is reset. With those source
 * dependencies recovered, ordinary G8 remains five address words away, while
 * the existing split profile materializes the common pose base exactly.
 *
 * The global active-key guard remains separate from the input key, and the
 * initial threshold still tests the input magnitude against 0x4000, not a
 * replacement `min(scale, 0x4000)` expression. All six canonical attempts are
 * preserved; one post-terminal resolution records this result. No register
 * pins, source-level inline assembly, symbol aliases, or new profile is used.
 */
void Model_PrepareEffectKey(Key *state)
{
    /* The copier writes four halfwords; only the first three are coordinates. */
    s16 slot_position[4];
    s16 spline_samples[10][3];
    Coeff *endpoint_view;
    SVECTOR *pose_channel;
    s32 channel_index;
    s32 sample_index;
    s32 magnitude;
    s32 duration_scale;
    s32 sampled_distance;
    u32 duration_bound;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 slot_index;
    u16 radius_halfword;

    {
        s32 signed_magnitude;
        s32 positive_magnitude;
        signed_magnitude = state->magnitude;
        magnitude = __builtin_abs(signed_magnitude);
        positive_magnitude = magnitude;
        if (magnitude <= 0) {
            positive_magnitude = 1;
        }
        duration_scale = positive_magnitude * 2;
    }
    Model_RefreshEffectKeySlotValues(state);
    state->progress = 0;
    if (gModel_pCurrentEffectKey->ready != 0) {
        return;
    }
    state->ready = 1;
    if (magnitude >= 0x4000) {
        state->duration = 0x4000;
    } else {
        state->duration = duration_scale;
    }
    if (state->magnitude >= 0) {
        return;
    }

    /* Coeff preserves the aligned halfword loads of these packed endpoints. */
    endpoint_view = (Coeff *)state;
    for (channel_index = 0; channel_index < 2; channel_index++, endpoint_view++) {
        pose_channel = &gModel_aPoseSnapshot[channel_index];
        switch ((s16)endpoint_view->w) {
        case 0x80:
        case 0x81:
        {
            s32 dx, dy, dz;
            slot_index = (s16)(endpoint_view->w & 0xFF7F);
            if (Model_GetSlotReadiness(slot_index) != 1) {
                continue;
            }
            Model_CopySlotPosition(slot_index, (u16 *)slot_position);
            slot_position[0] = slot_position[0] + (u16)endpoint_view->x;
            slot_position[1] = slot_position[1] + (u16)endpoint_view->y;
            slot_position[2] = slot_position[2] + (u16)endpoint_view->z;
            dx = slot_position[0] - pose_channel->vx;
            dy = slot_position[1] - pose_channel->vy;
            dz = slot_position[2] - pose_channel->vz;
            duration_bound = duration_scale * SquareRoot0(dx * dx + dy * dy + dz * dz) / 1000;
            if (state->duration < duration_bound) {
                state->duration = duration_bound;
            }
            continue;
        }
        case 1:
            sampled_distance = 0;
            for (sample_index = 0; sample_index < 10; sample_index++) {
                Model_EvaluateKeyframeSpline(state, channel_index, sample_index, 0x1E, spline_samples[sample_index]);
            }
            for (sample_index = 1; sample_index < 10; sample_index++) {
                dx = spline_samples[sample_index - 1][0] - spline_samples[sample_index][0];
                dy = spline_samples[sample_index - 1][1] - spline_samples[sample_index][1];
                dz = spline_samples[sample_index - 1][2] - spline_samples[sample_index][2];
                sampled_distance += SquareRoot0(dx * dx + dy * dy + dz * dz);
            }
            duration_bound = duration_scale * sampled_distance / 1000;
            if (state->duration < duration_bound) {
                state->duration = duration_bound;
            }
            continue;
        case 4:
        {
            s32 radius_scaled;
            s32 endpoint_y_magnitude;
            radius_halfword = (u16)gModel_aPoseSnapshot[2].vx;
            if (gModel_aPoseSnapshot[2].vx < endpoint_view->x) {
                radius_halfword = (u16)endpoint_view->x;
            }
            radius_scaled = (s16)radius_halfword * 6284 / 1000;
            duration_bound = duration_scale * radius_scaled;
            endpoint_y_magnitude = __builtin_abs(endpoint_view->y);
            duration_bound = (u32)(duration_bound * endpoint_y_magnitude) / 4096000;
            if (state->duration < duration_bound) {
                state->duration = duration_bound;
            }
            continue;
        }
        default:
            continue;
        }
    }
}

/* Evaluates one channel of the keyframe ring that Model_ApplyEffectKeyQueueCommand fills and
   Model_CaptureKeyframePose seeds. `current_key` points at a 0x28-byte key inside gModel_aEffectKeys, so
   `current_key - gModel_aEffectKeys` is the ring index; the three keys the segment spans are
   that index and the next two modulo the live key count in gModel_bEffectKeyCount. Keys
   whose 0x8-byte channel record has kind 1 at +6 terminate the segment, and
   the tail is padded with the last real key. The four control points handed to
   Model_BuildCubicCoefficients are the current pose at gModel_aPoseSnapshot followed by those three
   keys, except that a non-zero `time_denominator` also replaces the first with the previous
   key when that one is kind 1. With `time_denominator` zero the divisor is instead the sum
   of the distinct keys' +0x22 durations. Each of the three components is then
   Horner-evaluated at time_numerator/time_denominator and added to the constant term. */
void Model_EvaluateKeyframeSpline(Key *current_key, s32 channel_index, s32 time_numerator, s32 time_denominator, s16 *output_xyz)
{
    Key *segment_keys[3] = {
        &gModel_aEffectKeys[(current_key - gModel_aEffectKeys) % gModel_bEffectKeyCount],
        &gModel_aEffectKeys[(current_key - gModel_aEffectKeys + 1) % gModel_bEffectKeyCount],
        &gModel_aEffectKeys[(current_key - gModel_aEffectKeys + 2) % gModel_bEffectKeyCount]
    };
    s16 *control_points[4];
    Coeff axis_coefficients[3];
    s32 point_or_axis_index;
    s32 tail_source_index;

    memset(control_points, 0, 16);
    control_points[0] = (s16 *)(
        (u8 *)gModel_aPoseSnapshot + (u32)&((u8 (*)[8])0)[channel_index]);
    for (point_or_axis_index = 1; point_or_axis_index < 3; point_or_axis_index++) {
        s16 *next_channel_halfwords;

        next_channel_halfwords = (s16 *)(
            KEY_BYTES(segment_keys[point_or_axis_index]) + (u32)&((u8 (*)[8])0)[channel_index]);
        if (next_channel_halfwords[3] != 1) {
            break;
        }
    }
    tail_source_index = point_or_axis_index - 1;
    if (point_or_axis_index < 3) {
        for (; point_or_axis_index < 3; point_or_axis_index++) {
            segment_keys[point_or_axis_index] = segment_keys[tail_source_index];
        }
    }
    for (point_or_axis_index = 0; point_or_axis_index < 3; point_or_axis_index++) {
        control_points[point_or_axis_index + 1] = (s16 *)(
            KEY_BYTES(segment_keys[point_or_axis_index]) + (u32)&((u8 (*)[8])0)[channel_index]);
    }
    if (time_denominator != 0) {
        Key *previous_key;
        s16 *previous_channel;

        previous_key = &gModel_aEffectKeys[(current_key - gModel_aEffectKeys + gModel_bEffectKeyCount - 1) % gModel_bEffectKeyCount];
        previous_channel = (s16 *)(KEY_BYTES(previous_key) + (u32)&((u8 (*)[8])0)[channel_index]);
        if (previous_channel[3] == 1) {
            control_points[0] = previous_channel;
        }
        point_or_axis_index = 0;
    } else {
        time_denominator = segment_keys[0]->duration;
        for (point_or_axis_index = 1; point_or_axis_index < 3; point_or_axis_index++) {
            if (segment_keys[point_or_axis_index - 1] != segment_keys[point_or_axis_index]) {
                time_denominator += segment_keys[point_or_axis_index]->duration;
            }
        }
        point_or_axis_index = 0;
    }
    for (point_or_axis_index = 0; point_or_axis_index < 3; point_or_axis_index++) {
        s32 polynomial_value;

        Model_BuildCubicCoefficients(control_points[0][point_or_axis_index], control_points[1][point_or_axis_index], control_points[2][point_or_axis_index], control_points[3][point_or_axis_index], &axis_coefficients[point_or_axis_index], 2);
        polynomial_value = axis_coefficients[point_or_axis_index].x * time_numerator / time_denominator;
        polynomial_value = (polynomial_value + axis_coefficients[point_or_axis_index].y) * time_numerator / time_denominator;
        polynomial_value = (polynomial_value + axis_coefficients[point_or_axis_index].z) * time_numerator / time_denominator;
        output_xyz[point_or_axis_index] = axis_coefficients[point_or_axis_index].w + polynomial_value;
    }
}

/* Refreshes the current pose at gModel_aPoseSnapshot that the evaluator above uses as
   its first control point, from the two source records returned by
   Model_GetCameraViewBuffer and Model_GetViewMetricsBuffer, and optionally
   refreshes up to two resolved endpoints in gModel_pCurrentEffectKey from model slots.
   This is the seed half of the ring the evaluator reads, which is why the two share a
   translation unit. */
void Model_CaptureKeyframePose(s32 capture_endpoints)
{
    GsRVIEW2 *view = Model_GetCameraViewBuffer();
    ModelViewMetrics *metrics = Model_GetViewMetricsBuffer();

    gModel_aPoseSnapshot[0].vx = view->vpx;
    gModel_aPoseSnapshot[0].vy = view->vpy;
    gModel_aPoseSnapshot[0].vz = view->vpz;
    gModel_aPoseSnapshot[1].vx = view->vrx;
    gModel_aPoseSnapshot[1].vy = view->vry;
    gModel_aPoseSnapshot[1].vz = view->vrz;
    gModel_aPoseSnapshot[2].vx = metrics->distance;
    gModel_aPoseSnapshot[2].vy = metrics->yaw;
    gModel_aPoseSnapshot[2].vz = metrics->pitch;
    if (capture_endpoints != 0) {
        Key *key = gModel_pCurrentEffectKey;
        if (key != (Key *)0) {
            s32 endpoint_index = 0;
            s32 resolved_byte_offset = 16;
            /* A byte cursor over key->requested: ModelEffectEndpoint is
               packed, so reading its members costs byte loads (+0x30). */
            u8 *requested_cursor = KEY_BYTES(key);
            for (; endpoint_index < 2; resolved_byte_offset += 8, endpoint_index++, requested_cursor += 8) {
                s32 endpoint_kind = *(s16 *)(requested_cursor + 6);
                if (endpoint_kind < 4) {
                    if (endpoint_kind >= 2)
                        Model_CopySlotPosition(
                            *(s16 *)requested_cursor, (u16 *)(KEY_BYTES(key) + resolved_byte_offset)
                        );
                }
            }
        }
    }
}

s32 Model_GetCameraHoldState(void)
{
    return gModel_bCameraHoldState;
}

void Model_SetCameraHoldState(s32 value)
{
    gModel_bCameraHoldState = value;
}

s32 Model_GetHandlerCallbackState(void)
{
    return gModel_bHandlerCallbackState;
}

void Model_SetHandlerCallbackState(s32 value)
{
    gModel_bHandlerCallbackState = value;
}

ModelCameraOrbitPreset *ModelEffect_GetCoefficient(s32 index)
{
    return &gModel_aCameraOrbitPresets[index];
}

s32 Model_AdjustCameraDistanceForSlot(s32 slot_selector, s32 requested_distance)
{
    ModelEffectAdjustment dimensions;
    s16 size_delta;

    if (slot_selector >= 2) {
        return requested_distance;
    }

    Model_GetSlotDimensionsWithOverrides(slot_selector, (s16 *)&dimensions);

    if (dimensions.max < 50) {
        dimensions.max = 50;
    }

    dimensions.max -= 300;
    size_delta = dimensions.max;

    if (size_delta != 0) {
        s32 divisor = 750;
        s32 distance_basis = requested_distance;

        if (size_delta > 0) {
            distance_basis = (s32)(requested_distance + ((u32)requested_distance >> 31)) >> 1;
        }
        requested_distance += (size_delta * distance_basis) / divisor;
    }

    return requested_distance;
}

void Model_SetCameraOrbitFromPreset(s32 slot_selector, s32 preset_index, SVECTOR *target_offset)
{
    ModelCameraOrbitPreset *preset;
    s32 distance;
    s32 size_delta;
    s32 distance_work;
    s32 size_divisor;
    s32 size_product;
    ModelEffectAdjustment dimensions;

    preset = &gModel_aCameraOrbitPresets[preset_index];

    if (gModel_bCameraHoldState == 1) {
        if (gModel_bHandlerCallbackState == 1) {
            return;
        }
    }

    distance = preset->distance;

    if (slot_selector < 2) {
        Model_GetSlotDimensionsWithOverrides(slot_selector, (s16 *)&dimensions);
        if (dimensions.max < 0x32) {
            dimensions.max = 0x32;
        }
        dimensions.max -= 0x12C;
        size_delta = dimensions.max;
        if (size_delta != 0) {
            size_divisor = 750;
            distance_work = distance;
            if (size_delta > 0) {
                distance_work = distance / 2;
            }
            size_product = size_delta * distance_work;
            distance_work = size_product;
            distance += distance_work / size_divisor;
        }
    }

    Model_SetCameraOrbit(slot_selector, distance, preset->yaw, preset->pitch, target_offset);
}

void Model_SetCameraOrbit(int slot_selector, int distance, int yaw, int pitch, SVECTOR *target_offset)
{
    GsRVIEW2 *camera_view = Model_GetCameraViewBuffer();
    ModelViewMetrics *view_metrics = Model_GetViewMetricsBuffer();
    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == 1)
        return;
    if (slot_selector >= 0) {
        short slot_position[4];
        Model_CopySlotPosition(slot_selector, (u16 *)slot_position);
        camera_view->vrx = slot_position[0];
        camera_view->vry = slot_position[1];
        camera_view->vrz = slot_position[2];
    }
    if (target_offset) {
        int offset_sign = -1;
        if (slot_selector <= 0)
            offset_sign = 1;
        camera_view->vrx += target_offset->vx * offset_sign;
        camera_view->vry += target_offset->vy;
        camera_view->vrz += target_offset->vz * offset_sign;
    }
    view_metrics->distance = distance;
    view_metrics->yaw = (yaw + MODEL_ANGLE_FULL_TURN +
            (slot_selector <= 0 ? 3 * MODEL_ANGLE_QUARTER_TURN :
                         MODEL_ANGLE_QUARTER_TURN)) %
        MODEL_ANGLE_FULL_TURN;
    view_metrics->pitch = (pitch + MODEL_ANGLE_FULL_TURN) % MODEL_ANGLE_FULL_TURN;
    Model_StepViewOrbit(1, MODEL_ANGLE_FULL_TURN, 0, 0, 1);
    Model_CaptureKeyframePose(1);
    Model_RequestCameraMove(-1);
}

int Model_NextEffectKeyQueueMode(void)
{
    if (gModel_nEffectKeyBatchOrdinal < 0) {
        return 0;
    }
    return gModel_nEffectKeyBatchOrdinal++ > 0;
}

void Model_SetEffectKeyBatchingEnabled(int value)
{
    if (gModel_bCameraHoldState != 1 || gModel_bHandlerCallbackState != 1) {
        if (value == 0) {
            gModel_nEffectKeyBatchOrdinal = -1;
        } else {
            gModel_nEffectKeyBatchOrdinal = 0;
        }
    }
}

/* Applies the same slot-dependent camera-distance adjustment, then queues
   relative orbit endpoints through Model_QueueRelativeCameraOrbit. */
void Model_QueueCameraOrbitFromPreset(s32 slot_selector, s32 preset_index, SVECTOR *target_offset, s32 key_magnitude) {
    ModelCameraOrbitPreset *preset = &gModel_aCameraOrbitPresets[preset_index];
    s32 distance;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == gModel_bCameraHoldState) {
        return;
    }

    distance = preset->distance;

    if (slot_selector < 2) {
        ModelEffectAdjustment dimensions;
        s32 size_delta;

        Model_GetSlotDimensionsWithOverrides(slot_selector, (s16 *)&dimensions);
        if (dimensions.max < 0x32) {
            dimensions.max = 0x32;
        }
        dimensions.max -= 300;
        size_delta = dimensions.max;
        if (size_delta != 0) {
            s32 divisor = 750;
            s32 distance_basis = (size_delta > 0) ? (distance / 2) : distance;
            distance += (size_delta * distance_basis) / divisor;
        }
    }

    Model_QueueRelativeCameraOrbit(slot_selector, distance, preset->yaw, preset->pitch, target_offset, key_magnitude);
}

void Model_QueueEffectEndpointPair(s32 a, s32 b, s32 c)
{
    ModelEffectEndpoint *first_endpoint =
        a < 0 ? 0 : &gModel_aEffectEndpoints[a];
    ModelEffectEndpoint *second_endpoint =
        b < 0 ? 0 : &gModel_aEffectEndpoints[b];
    s32 ordinal_or_mode;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == 1) {
        return;
    }
    ordinal_or_mode = (s8)gModel_nEffectKeyBatchOrdinal;
    if (ordinal_or_mode < 0) {
        ordinal_or_mode = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal++;
        ordinal_or_mode = ordinal_or_mode > 0;
    }
    Model_ApplyEffectKeyQueueCommand(ordinal_or_mode, first_endpoint, second_endpoint, c);
}

void Model_QueueCameraShake(s32 value, s32 arg)
{
    /* The aligned array keeps retail's four halfword stores; a packed local
       would expand them into byte stores. */
    s16 shake_halfwords[4] = {value, 0, 0, 5};
    s32 ordinal_or_mode;
    s32 unsigned_ordinal;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == 1) {
        return;
    }
    ordinal_or_mode = (s8)gModel_nEffectKeyBatchOrdinal;
    unsigned_ordinal = (u8)gModel_nEffectKeyBatchOrdinal;
    if (ordinal_or_mode < 0) {
        ordinal_or_mode = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = unsigned_ordinal + 1;
        ordinal_or_mode = ordinal_or_mode > 0;
    }
    Model_ApplyEffectKeyQueueCommand(
        ordinal_or_mode, MODEL_EFFECT_ENDPOINT_VIEW(&shake_halfwords), MODEL_EFFECT_ENDPOINT_VIEW(&shake_halfwords), arg
    );
}

void Model_QueueEffectEndpointSequences(
    s32 count, ModelEffectEndpoint *p1, ModelEffectEndpoint *p2, s32 magnitude
)
{
    s8 batch_ordinal;
    s32 queue_mode;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == gModel_bCameraHoldState) {
        return;
    }

    batch_ordinal = gModel_nEffectKeyBatchOrdinal;
    if (batch_ordinal < 0) {
        queue_mode = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = (u8)gModel_nEffectKeyBatchOrdinal + 1;
        queue_mode = batch_ordinal > 0;
    }

    if (count == 0) {
        return;
    }

    do {
        Model_ApplyEffectKeyQueueCommand(
            queue_mode,
            p1,
            p2,
            magnitude
        );

        if (p1 != 0) {
            p1 = p1 + 1;
        }
        if (p2 != 0) {
            p2 = p2 + 1;
        }

        if ((p1 == 0 || *(s16 *)&p1->kind == 0) &&
            (p2 == 0 || *(s16 *)&p2->kind == 0)) {
            return;
        }
        if (count > 0) {
            count--;
        }
        queue_mode = 1;
    } while (count != 0);
}

void Model_ApplyEffectKeyQueueCommand(
    s32 mode,
    ModelEffectEndpoint *first,
    ModelEffectEndpoint *second,
    s32 magnitude
)
{
    u8 *prepare_key_bytes;
    u8 *next_key_bytes;
    u8 *append_key_bytes;
    s32 queued_count;
    s32 key_index;
    s32 one;

    one = 1;
    if (gModel_bCameraHoldState == one && gModel_bHandlerCallbackState == one) {
        return;
    }

    switch (mode) {
    case 0:
        Model_RequestCameraMove(-1);
        gModel_pCurrentEffectKey = gModel_aEffectKeys;
        gModel_bEffectKeyCount = 0;
        if (first == (ModelEffectEndpoint *)0 &&
            second == (ModelEffectEndpoint *)0 &&
            (magnitude == 0 || magnitude == 0x4000)) {
            return;
        }
        Model_CaptureKeyframePose(0);
        gModel_bEffectKeyPreparationPending = 1;
        /* fall through */

    case 1:
        if (gModel_bEffectKeyCount < 0xA) {
            Key *key = &gModel_aEffectKeys[gModel_bEffectKeyCount];
            append_key_bytes = KEY_BYTES(key);
            if (first != (ModelEffectEndpoint *)0) {
                key->requested[0] = *first;
            } else {
                key->requested[0].kind = 0;
            }
            if (second != (ModelEffectEndpoint *)0) {
                key->requested[1] = *second;
                key->ready = 0;
            } else {
                key->requested[1].kind = 0;
                key->ready = 0;
            }
            queued_count = gModel_bEffectKeyCount;
            key->magnitude = magnitude;
            key->duration = 0;
            key->progress = 0;
            gModel_bEffectKeyCount = queued_count + 1;
        }
        break;

    case 2:
        next_key_bytes = KEY_BYTES(gModel_aEffectKeys);
        for (key_index = 0; key_index < gModel_bEffectKeyCount; key_index++) {
            prepare_key_bytes = next_key_bytes;
            next_key_bytes += 0x28;
            Model_PrepareEffectKey((Key *)prepare_key_bytes);
        }
        gModel_bEffectKeyPreparationPending = 0;
        gModel_nEffectKeyBatchOrdinal = -1;
        break;
    }
}

void Model_ResetEffectKeyQueue(void)
{
    gModel_pCurrentEffectKey = 0;
    gModel_bEffectKeyCount = 0;
    gModel_bEffectKeyPreparationPending = 0;
    gModel_nEffectKeyBatchOrdinal = -1;
    gModel_bCameraHoldState = 0;
    gModel_bHandlerCallbackState = 0;
}

int Model_IsEffectKeyQueueEmpty(void)
{
    return gModel_bEffectKeyCount == 0;
}

int Model_GetCurrentEffectKeyAddress(void)
{
    int value = 0;

    if (gModel_bEffectKeyCount != 0) {
        value = (s32)gModel_pCurrentEffectKey;
    }
    return value;
}

void Model_RefreshEffectKeySlotValues(Key *key)
{
    u8 *data = KEY_BYTES(key);
    int i;

    if (!data) {
        data = KEY_BYTES(gModel_pCurrentEffectKey);
    }
    if (!data) {
        return;
    }
    for (i = 0; i < 2; i++) {
        ModelEffectEndpoint *item =
            (ModelEffectEndpoint *)(data + i * sizeof(ModelEffectEndpoint));

        /* Key storage is halfword-aligned; packed member reads use bytes. */
        if (*(s16 *)&item->kind < 4) {
            if (*(s16 *)&item->kind >= 2) {
                Model_CopySlotPosition(
                    *(s16 *)&item->x,
                    (u16 *)(data + 0x10 + i * sizeof(u16[4]))
                );
            }
        }
    }
}

void Model_BuildCubicCoefficients(int a, int b, int c, int d, Coeff *out, int scale)
{
    int x = (b - c) * scale;
    int y = (a + c - b * 2) * scale;
    int z = (b - a) * scale;
    out->w = a;
    out->x = x - a + d;
    out->y = y;
    out->z = z;
}
