#include "../types.h"

#define SDSECONDARYSTATE_CUSTOM_EXTERN
#include "sound.h"

/* Value of an object's +0x183 byte when it owns no record. */
#define SD_SECONDARY_RECORD_NONE 0x63

extern u8 *D_8009B458;

extern s32 func_8004A8E4(s32 index, s32 value);

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
