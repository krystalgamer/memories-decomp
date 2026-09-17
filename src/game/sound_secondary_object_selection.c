#include "../types.h"
#include "sound.h"
#include "sound_secondary_object_selection.h"

s32 func_8004A854(s32 value)
{
    s32 result = -1;
    SDSecondaryState *state = D_8009B458;
    s32 best = 0xFFFF;
    s32 i = 0;

    if (state->object_count > i) {
        s32 offset;

        value = (u8)value;
        offset = 0;
        do {
            SDSecondaryState *entry =
                (SDSecondaryState *)((u8 *)state + offset);
            u16 candidate = entry->objects[0].field_001E;

            if ((u16)best >= candidate &&
                entry->objects[0].channel_index == value &&
                entry->objects[0].field_000D != 0) {
                best = candidate;
                result = i;
            }
            offset += SD_SECONDARY_OBJECT_SIZE;
            state = D_8009B458;
            i++;
        } while (i < state->object_count);
    }
    best = result;
    return best;
}

s32 func_8004A8E4(s32 index, s32 value)
{
    SDSecondaryState *state = D_8009B458;
    SDSecondaryObject *object = &state->objects[index];

    if (object->channel_index != SD_SECONDARY_RECORD_NONE) {
        SDSecondaryRecord *record = &state->channels[object->channel_index];

        if ((record->field_0006 & 0xF) != 0) {
            record->field_0006 = record->field_0006 - 1;
        }
    }
    return index;
}

s32 SD_SelectSecondaryObject(s32 value, s32 variant)
{
    SDSecondaryState *state = D_8009B458;
    s32 i;
    s32 result = -1;
    u32 best;

    /* Single-pass nesting preserves the retail allocation under GCC 2.8.1. */
    do {
        do {
            do {
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
            } while (0);
        } while (0);
    } while (0);

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
