#include "../types.h"
#include "sound.h"

int func_8004A854(int value)
{
    register int result asm("$9") = -1;
    u8 *state = (u8 *)D_8009B458;
    register short best asm("$8");
    register int i asm("$5");
    asm volatile("li %0,0xffff" : "=r"(best));
    if (*(short *)(state + 0x510) > 0) {
        register int offset asm("$7");
        int count;
        i = 0;
        value = (u8)value;
        offset = i;
        do {
            u8 *entry = state + offset;
            register u16 candidate asm("$6") =
                *(u16 *)(entry + 0x19E);
            if ((u16)best >= candidate && entry[0x183] == value &&
                entry[0x18D] != 0) {
                best = candidate;
                result = i;
            }
            state = (u8 *)D_8009B458;
            count = *(short *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    return result;
}

s32 func_8004A8E4(s32 index, s32 value)
{
    SDSecondaryState *state = D_8009B458;
    SDSecondaryObject *object = &state->objects[index];

    if (object->channel_index != SD_SECONDARY_RECORD_NONE) {
        SDSecondaryRecord *record =
            (SDSecondaryRecord *)state + object->channel_index;

        if ((record->field_0006 & 0xF) != 0) {
            record->field_0006 = record->field_0006 - 1;
        }
    }
    return index;
}

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
