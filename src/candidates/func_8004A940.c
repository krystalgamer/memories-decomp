/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_secondary_object_selection.c.
 */
#include "../types.h"
#include "../game/sound.h"
#include "../game/sound_secondary_object_selection.h"

s32 func_8004A940(s32 value, s32 variant)
{
    SDSecondaryState *state;
    s32 i;
    register s32 result __asm__("$10") = -1;
    u32 best;

    state = D_8009B458;
    i = 0;
    if (state->object_count > 0) {
        s32 none = SD_SECONDARY_RECORD_NONE;
        s32 count = state->object_count;
        do {
            if (state->objects[i].channel_index == none) {
                return i;
            }
            i++;
        } while (i < count);
    }

    state = D_8009B458;
    i = 0;
    if (state->object_count > 0) {
        s32 count = state->object_count;
        do {
            if (state->objects[i].field_000D == 0) {
                return func_8004A8E4(i, (u8)value);
            }
            i++;
        } while (i < count);
    }

    state = D_8009B458;
    i = 0;
    if (state->object_count > 0) {
        do {
            u8 owner = state->objects[i].channel_index;
            if (owner == (u8)value &&
                state->objects[i].field_0005 == (u8)variant) {
                return func_8004A8E4(i, owner);
            }
            state = D_8009B458;
            i++;
        } while (i < state->object_count);
    }

    state = D_8009B458;
    i = 0;
    if (state->object_count > 0) {
        s32 count = state->object_count;
        do {
            if (state->objects[i].field_000D == 0) {
                return func_8004A8E4(i, (u8)value);
            }
            i++;
        } while (i < count);
    }

    state = D_8009B458;
    best = 0xFFFF;
    i = 0;
    if (state->object_count > 0) {
        s32 count = state->object_count;
        do {
            u16 candidate = state->objects[i].field_001E;
            if (candidate < best) {
                best = candidate;
                result = i;
            }
            i++;
        } while (i < count);
    }
    if (best >= 0xFFFF) {
        return -1;
    }
    return func_8004A8E4(result, (u8)value);
}
