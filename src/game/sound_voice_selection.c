#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

/*
 * SD_ApplyVoiceSlotVolume: voice-slot attribute initialization order
 *
 * The 136-byte routine at `0x80047864` matches all 34 instructions with the
 * existing uniform `gcc_2_8_1_g0` profile. It uses the incoming index directly
 * and the shared `SDValue` voice-volume and gain arrays, replacing five
 * register bindings, byte-cursor accesses, and an explicit assembly move. Its
 * `void(s32)` contract and `SpuSetVoiceAttr` call are unchanged.
 *
 * Initializing the attribute-mask field at `+0x3C8` before the voice-mask field
 * at `+0x3C4` in C gives GCC the required register allocation. GCC schedules
 * the resulting machine stores in retail order, with the voice-mask store
 * first. Reversing those source assignments changes five instruction words. A
 * split profile alone leaves two wrong high-address register words, so the
 * accepted source does not add a compiler profile or change the global
 * declaration.
 *
 * The conditional voice-mask shift, both unsigned fixed-point volume products,
 * their eight-bit shifts and halfword stores, and the final SDK attribute
 * publication remain intact.
 */
void SD_ApplyVoiceSlotVolume(s32 index)
{
    u32 mask = SD_VOICE_SLOT_MASK_BASE;
    SDValue *state;
    u32 product;

    if (index != 0) {
        mask <<= index;
    }
    state = g_SDValue;
    state->field_03C8 = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
    state->field_03C4 = mask;
    {
        u32 first = state->voice_volume_left[index];
        product = first * (u32)state->voice_value[index];
    }
    state->field_03CC = product >> 8;
    product = state->voice_volume_right[index] * (u32)state->voice_value[index];
    state->field_03CE = product >> 8;
    SpuSetVoiceAttr((SpuVoiceAttr *)&state->field_03C4);
}

void SD_UpdateVoiceSlots(void)
{
    s32 mask;
    s32 bit2;
    s32 accum;
    s32 i;
    s32 v0;
    u16 timer;

    mask = SD_VOICE_SLOT_MASK_BASE;
    bit2 = 1;
    accum = 0;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_step[i] != 0) {
            if (g_SDValue->voice_step[i] >= g_SDValue->voice_value[i]) {
                g_SDValue->voice_value[i] = 0;
            } else {
                g_SDValue->voice_value[i] =
                    g_SDValue->voice_value[i] - g_SDValue->voice_step[i];
            }
            if (g_SDValue->voice_value[i] != 0) {
                SD_ApplyVoiceSlotVolume(i);
                goto tail;
            }
            g_SDValue->voice_active_mask &= ~bit2;
            g_SDValue->voice_step[i] = 0;
            SpuSetKey(SPU_OFF, mask);
            accum |= mask;
        }
    tail:
        timer = g_SDValue->voice_timer[i];
        if (timer != 0) {
            timer -= 1;
            g_SDValue->voice_timer[i] = timer;
            if (timer == 0) {
                v0 = SpuGetKeyStatus(mask);
                if (v0 != SPU_OFF) {
                    SpuSetKey(SPU_OFF, mask);
                    accum |= mask;
                }
            }
        }
        v0 = SpuGetKeyStatus(mask);
        if (v0 == SPU_ON_ENV_OFF && !(accum & mask)) {
            SpuSetKey(SPU_OFF, mask);
            accum |= mask;
        }
        bit2 <<= 1;
        mask <<= 1;
    }
    if (accum != 0)
        SpuSetKey(SPU_OFF, accum);
}

void func_80047A68(void)
{
    s32 i;
    u32 mask = SD_VOICE_SLOT_MASK_BASE;
    u32 result = 0;
    u16 threshold = g_SDValue->field_0004;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_ids[i] >= threshold)
            result |= mask;
        mask <<= 1;
    }
    SpuSetKey(SPU_OFF, result);
}

s32 func_80047AD0(s32 value)
{
    u16 index = value;
    SDValue *state = g_SDValue;

    if (state->field_0448[index].field_0004 == 0)
        return 1;
    if (state->field_0442 == index)
        return 1;
    func_80047A68();
    SD_ResetPendingEntries();
    {
        register SDValue *final = g_SDValue;
        register s32 call_value = index;
        final->field_0442 = value;
        final->flags_0040 |= 2;
        SD_QueueValueLinkTransfer(call_value);
    }
    return 1;
}

s32 SD_RequestValueLink(u16 value)
{
    SDValue *state = g_SDValue;

    if (state->field_0442 == value)
        return 1;
    state->field_0442 = value;
    state->flags_0040 |= 2;
    SD_QueueValueLinkTransfer(value);
    return 1;
}

void func_80047BB4(u16 *items, s32 count)
{
    s32 i;

    SpuSetKey(SPU_OFF, SD_VOICE_SLOT_MASK_ALL);
    SD_ResetPendingEntries();
    g_SDValue->flags_0040 |= 2;
    for (i = 0; i < count; i++) {
        if (items[i] != 0xFFFF)
            SD_QueueValueLinkTransfer(items[i]);
    }
}
