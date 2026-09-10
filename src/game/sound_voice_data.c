#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_mix.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

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

void func_80048D08(s32 side, u32 *src)
{
    SDSeqBlock *blk;
    SDSeqBlock *other;
    SDSeqBlock *cur;
    SDNote *e;
    u32 addr_side;
    u32 addr_other;
    s32 rev;
    s32 i;
    s32 j;

    if ((u32)side >= 2) {
        return;
    }

    rev = 1 - side;
    addr_side = 0xD810 + (side % 2) * 0x19000;
    addr_other = 0xD810 + (rev % 2) * 0x19000;
    blk = (SDSeqBlock *)(0x801E7800 + (side << SD_PENDING_INPUT_BLOCK_SHIFT));
    other = (SDSeqBlock *)(0x801E7800 + (rev << SD_PENDING_INPUT_BLOCK_SHIFT));
    func_80048C70((u32 *)blk, src);
    func_8004763C();

    for (i = 0; i < 2; i++) {
        cur = other;
        if (i != 0) {
            cur = blk;
        }
        for (j = 0; j < cur->count; j++) {
            u16 key = cur->keys[j];
            if (key != SD_PENDING_ENTRY_NONE) {
                u16 n = g_SDValue->field_0440;
                u16 v;

                g_SDValue->field_043C[key] = n;
                g_SDValue->field_0444[n] = cur->data[j];
                e = (SDNote *)(n * 8 + (u32)g_SDValue->field_0444);
                v = *(u16 *)((u8 *)e + 6);
                *(u16 *)((u8 *)e + 6) =
                    (i != 0) ? (u16)(v + (addr_side >> 4))
                             : (u16)(v + (addr_other >> 4));
                g_SDValue->field_0440 = g_SDValue->field_0440 + 1;
            }
            if (i != 0) {
                g_SDValue->field_044C[side][j] = key;
            }
        }
    }
}

#include "sound_init.h"
#include "sound_voice_data.h"

