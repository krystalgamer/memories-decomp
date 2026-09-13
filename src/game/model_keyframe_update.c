/*
 * Evaluates the two camera keyframe channels, then resolves relative orbits.
 * Timing sources are live after callbacks, but the first timing destination
 * and the output view are captured. The slot copier writes four halfwords;
 * position and target retain that complete extent.
 *
 * All 3,172 bytes match with the existing uniform G8 split/no-strength-reduce
 * profile. The ordinary profile creates an extra pose-component induction
 * pointer and spills the evaluated-buffer pointer. The middle-axis helper
 * retains the signed adjustment's publication boundary; its arithmetic is
 * word-sized before the result is narrowed.
 *
 * Orbit projection uses the separate horizontal/vertical components already
 * used by func_80058434. The early elapsed local, remaining-duration capture
 * and opposite-view pointer cursor retain the original register lifetimes.
 * Finite valid key programs require nonzero interpolation/noise divisors.
 */
#include "../types.h"
#include "camera_view.h"
#include "func_8005EBF4.h"
#include "model_transfer_flags.h"
#include "model_effect_state.h"
#include "func_80058DD8.h"
#include "func_80058E1C.h"
#include "model_copy_slot_u16_values.h"
#include "model_update_view_metrics.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "func_80058434.h"
#include "../psyq/rand.h"

#define LIMIT(v, n) ((v) > 0 ? ((v) < (n) ? (v) : (n)) : \
    ((v) > -(n) ? (v) : -(n)))

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

void func_8005DBA4(void)
{
    s32 *outputs[2];
    s16 position[4];
    s16 evaluated[4];
    s16 target[4];
    s16 delta[4];
    s32 active;
    s32 persistent;
    s32 channel;
    s32 offset;
    s32 **output;
    s16 *pose;
    s16 *interpolated;

    outputs[0] = func_800591FC();
    outputs[1] = outputs[0] + 3;
    persistent = 0;
    active = 0;
    if (!D_8009B078 || !D_8009B074) {
        return;
    }
    if (D_8009B079 && (D_8009B07B != 1 || D_8009B07C != 1)) {
        Key *key = D_800F5788;
        s32 i;
        for (i = 0; i < D_8009B078; i++) {
            func_8005E808((u8 *)key++);
        }
        D_8009B079 = 0;
        D_8009B07A = -1;
    }
    if (*(u16 *)(D_8009B074 + 0x22) != 0x4000) {
        u8 *previous = D_8009B074;
        s32 step = func_80058E1C();
        s32 elapsed;
        if (*(u16 *)(D_8009B074 + 0x24) + step <
            *(u16 *)(D_8009B074 + 0x22)) {
            step = func_80058E1C();
            elapsed = *(u16 *)(D_8009B074 + 0x24) + step;
        } else {
            elapsed = *(u16 *)(D_8009B074 + 0x22);
        }
        *(u16 *)(previous + 0x24) = elapsed;
    }
    channel = 0;
    interpolated = evaluated;
    output = outputs;
    pose = (s16 *)D_800F5768;
    offset = 0;
    do {
        u8 *key = D_8009B074;
        s16 *record = (s16 *)(key + offset);
        if (record[3] == 0) {
            goto next_channel;
        }
        switch (record[3]) {
        case 0x80:
        case 0x81: {
            s32 slot = (s16)((u16)record[3] & 0xFF7F);
            s32 elapsed = *(u16 *)(key + 0x24);
            s32 duration = *(u16 *)(key + 0x22);
            s32 remaining;
            if (func_80058DD8(slot) != 1) {
                break;
            }
            Model_CopySlotU16Values(slot, (u16 *)position);
            {
                s16 *relative = (s16 *)(D_8009B074 + offset);
                position[0] += (u16)relative[0];
                position[1] += (u16)relative[1];
                position[2] += (u16)relative[2];
            }
            remaining = duration - elapsed;
            (*output)[0] = pose[0] * remaining / duration
                + position[0] * elapsed / duration;
            (*output)[1] = pose[1] * remaining / duration
                + position[1] * elapsed / duration;
            (*output)[2] = pose[2] * remaining / duration
                + position[2] * elapsed / duration;
            break;
        }
        case 1: {
            s32 j;
            s16 *component;
            s32 **destination;
            func_8005EBF4((Key *)key, channel,
                *(u16 *)(key + 0x24), 0, interpolated);
            j = 0;
            destination = output;
            component = interpolated;
            for (; j < 3; j++) {
                (*destination)[j] = *component++;
            }
            break;
        }
        case 2:
        case 3: {
            s32 slot;
            s32 amount;
            persistent++;
            slot = record[0];
            amount = record[1];
            if (func_80058DD8(slot) != 1) {
                break;
            }
            Model_CopySlotU16Values(slot, (u16 *)target);
            {
                s16 *following = (s16 *)(D_8009B074 + offset);
                delta[0] = target[0] - (u16)following[8];
                delta[1] = target[1] - (u16)following[9];
                delta[2] = target[2] - (u16)following[10];
                if (following[3] == 2) {
                    if (amount == 0) {
                        amount = 10;
                    }
                    {
                        s32 value = delta[0];
                        if (value > 0) {
                            value -= amount;
                            if (value < 0) {
                                value = 0;
                            }
                        } else {
                            value += amount;
                            if (value > 0) {
                                value = 0;
                            }
                        }
                        delta[0] = value;
                    }
                    {

                        delta[1] = Keyframe_ApplyDeadzone(&delta[1], amount);
                    }
                    {
                        s32 value = delta[2];
                        if (value > 0) {
                            value -= amount;
                            if (value < 0) {
                                value = 0;
                            }
                        } else {
                            value += amount;
                            if (value > 0) {
                                value = 0;
                            }
                        }
                        delta[2] = value;
                    }
                    delta[0] = LIMIT(delta[0], 60);
                    delta[1] = LIMIT(delta[1], 60);
                    delta[2] = LIMIT(delta[2], 60);
                    (*output)[0] += delta[0];
                    (*output)[1] += delta[1];
                    (*output)[2] += delta[2];
                    {
                        s16 *committed = (s16 *)(D_8009B074 + offset);
                        committed[8] += (u16)delta[0];
                        committed[9] += (u16)delta[1];
                        committed[10] += (u16)delta[2];
                    }
                } else {
                    if (amount == 0) {
                        amount = 2000;
                    }
                    delta[0] = LIMIT(delta[0], amount);
                    delta[1] = LIMIT(delta[1], amount);
                    delta[2] = LIMIT(delta[2], amount);
                    (*output)[0] = pose[0] + delta[0];
                    (*output)[1] = pose[1] + delta[1];
                    (*output)[2] = pose[2] + delta[2];
                }
            }
            break;
        }
        case 5: {
            s32 amount = record[0];
            s32 first;
            s32 second;
            persistent++;
            if (*(u16 *)(key + 0x24) >= *(u16 *)(key + 0x22)) {
                (*output)[0] = pose[0];
                (*output)[2] = pose[2];
                break;
            }
            first = rand();
            second = rand();
            first >>= 8;
            (*output)[0] = pose[0] + first % amount
                - ((second >> 8) % amount) / 2;
            first = rand();
            second = rand();
            first >>= 8;
            (*output)[2] = pose[2] + first % amount
                - ((second >> 8) % amount) / 2;
            break;
        }
        }
        active++;
next_channel:
        output++;
        pose += 4;
        channel++;
        offset += 8;
    } while (channel < 2);
    {
        s16 *orbit_pose;
        s32 **viewpoints;
        channel = 0;
        orbit_pose = (s16 *)D_800F5768;
        output = outputs;
        viewpoints = output;
        do {
            s32 elapsed;
            u8 *key = D_8009B074;
            s16 *record = (s16 *)(key + channel * 8);
            if (record[3] == 4) {
                s32 duration = (elapsed = *(u16 *)(key + 0x24),
                    *(u16 *)(key + 0x22));
                s32 yaw_step = record[1] * elapsed / duration;
                s32 pitch_step = record[2] * elapsed / duration;
                s32 radius = orbit_pose[8];
                s32 remaining = duration - elapsed;
                s32 yaw = yaw_step + orbit_pose[9];
                s32 pitch = pitch_step + orbit_pose[10];
                s32 cosine;
                s32 sine;
                s32 depth;
                s32 horizontal;
                s32 vertical;
                if (record[0] != 0) {
                    radius = (radius * remaining + record[0] * elapsed) / duration;
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
                    s32 **opposite = viewpoints;
                    opposite += channel ^ 1;
                    (*output)[0] = (*opposite)[0] + horizontal;
                    (*output)[1] = (*opposite)[1] + vertical;
                    (*output)[2] = (*opposite)[2] + depth;
                }
            }
            channel++;
            output++;
        } while (channel < 2);
    }
    if (!active && *(u16 *)(D_8009B074 + 0x22) == 0x4000) {
        D_8009B074 = 0;
        D_8009B078 = 0;
        return;
    }
    if (*(u16 *)(D_8009B074 + 0x24) >=
        *(u16 *)(D_8009B074 + 0x22)) {
        func_8005F070(0);
        D_8009B074 += 0x28;
        if ((Key *)D_8009B074 - D_800F5788 >= D_8009B078) {
            if (D_8009B078 >= 2) {
                D_8009B074 = (u8 *)D_800F5788;
                D_8009B074[0x26] = 0;
            } else {
                D_8009B074 = 0;
                D_8009B078 = 0;
            }
        }
        if (D_8009B074) {
            func_8005E808(D_8009B074);
            if (*(s16 *)(D_8009B074 + 0x20) == 0) {
                func_8005DBA4();
            }
        }
    }
    if (persistent == 2) {
        func_80058434(1, 0x1000, 0, 0, 0);
    } else {
        Model_UpdateViewMetrics(0);
    }
}
