#include "../types.h"

#define SDSECONDARYSTATE_CUSTOM_EXTERN
#include "sound.h"

extern u8 *D_8009B458;

int func_8004A854(int value)
{
    register int result asm("$9") = -1;
    u8 *state = D_8009B458;
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
            state = D_8009B458;
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
    SDSecondaryState *state = (SDSecondaryState *)D_8009B458;
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
    u8 *state;
    s32 i;
    register s32 result __asm__("$10") = -1;
    u32 best;

    state = D_8009B458;
    i = 0;
    if (*(s16 *)(state + 0x510) > 0) {
        s32 none = SD_SECONDARY_RECORD_NONE;
        s32 count = *(s16 *)(state + 0x510);
        do {
            if (state[0x183] == none) {
                return i;
            }
            i++;
            state += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }

    state = D_8009B458;
    i = 0;
    if (*(s16 *)(state + 0x510) > 0) {
        s32 count = *(s16 *)(state + 0x510);
        do {
            if (state[0x18D] == 0) {
                return func_8004A8E4(i, (u8)value);
            }
            i++;
            state += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }

    state = D_8009B458;
    i = 0;
    if (*(s16 *)(state + 0x510) > 0) {
        do {
            u8 *entry = state + i * SD_SECONDARY_OBJECT_SIZE;
            u8 owner = entry[0x183];
            if (owner == (u8)value && entry[0x185] == (u8)variant) {
                return func_8004A8E4(i, owner);
            }
            state = D_8009B458;
            i++;
        } while (i < *(s16 *)(state + 0x510));
    }

    state = D_8009B458;
    i = 0;
    if (*(s16 *)(state + 0x510) > 0) {
        s32 count = *(s16 *)(state + 0x510);
        do {
            if (state[0x18D] == 0) {
                return func_8004A8E4(i, (u8)value);
            }
            i++;
            state += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }

    state = D_8009B458;
    best = 0xFFFF;
    i = 0;
    if (*(s16 *)(state + 0x510) > 0) {
        u8 *entry;
        s32 count = *(s16 *)(state + 0x510);
        entry = state;
        do {
            u16 candidate = *(u16 *)(entry + 0x19E);
            if (candidate < best) {
                best = candidate;
                result = i;
            }
            i++;
            entry += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    if (best >= 0xFFFF) {
        return -1;
    }
    return func_8004A8E4(result, (u8)value);
}
