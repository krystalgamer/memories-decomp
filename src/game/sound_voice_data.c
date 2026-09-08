#include "../types.h"
#include "sound.h"

typedef char SDPendingInput_copy_groups_must_cover_block[
    SD_PENDING_INPUT_COPY_GROUP_COUNT * 8 * sizeof(u32) ==
        SD_PENDING_INPUT_BLOCK_SIZE ? 1 : -1
];

void func_80048C0C(u16 value, u8 enabled)
{
    s32 i;
    s32 mask;

    if (enabled == 0)
        return;
    mask = 1;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        SDValue *state = g_SDValue;
        if (state->voice_ids[i] == value &&
            (state->voice_active_mask & mask)) {
            state->voice_step[i] = enabled;
        }
    }
}

void func_80048C70(u32 *dst, u32 *src)
{
    u32 i;

    for (i = 0; i < SD_PENDING_INPUT_COPY_GROUP_COUNT; i++) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}
