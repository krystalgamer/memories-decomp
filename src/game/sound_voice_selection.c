#include "../types.h"
#include "../psyq/libspu.h"
#include "func_80047788.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

/*
 * func_80047864: voice-slot attribute initialization order
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
void func_80047864(s32 index)
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

void func_800478EC(void)
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
                func_80047864(i);
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
    func_8004763C();
    {
        register SDValue *final = g_SDValue;
        register s32 call_value = index;
        final->field_0442 = value;
        final->flags_0040 |= 2;
        func_80047788(call_value);
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
    func_80047788(value);
    return 1;
}

void func_80047BB4(u16 *items, s32 count)
{
    s32 i;

    SpuSetKey(SPU_OFF, SD_VOICE_SLOT_MASK_ALL);
    func_8004763C();
    g_SDValue->flags_0040 |= 2;
    for (i = 0; i < count; i++) {
        if (items[i] != 0xFFFF)
            func_80047788(items[i]);
    }
}

s32 func_80047C50(s32 value)
{
    u16 result = value;

    value &= 0x8000;
    if (value)
        return result;
    return 0xFFFF;
}

void SD_KeyOffVoice(s32 voice)
{
    s32 count;

    for (count = 0; count < SD_KEY_OFF_RETRY_LIMIT; count++) {
        SpuSetKey(SPU_OFF, voice);
        if (SpuGetKeyStatus(voice) == SPU_OFF)
            break;
    }
}

void func_80047CC4(s32 value)
{
    s32 normalized = func_80047C50((u16)value);
    SDValue *state = g_SDValue;
    s32 tries = 0;
    s32 slot = state->field_0435;
    s32 bit = 1 << (u8)slot;
    s32 command = 1 << ((u8)slot + SD_VOICE_SLOT_KEY_SHIFT);

    do {
        state = g_SDValue;
        if (state->voice_active_mask & bit) {
            if (state->voice_ids[(u8)slot] == (u16)normalized) {
                SD_KeyOffVoice(command);
                g_SDValue->field_0435 = slot;
                g_SDValue->voice_active_mask &= ~bit;
                break;
            }
        }
        bit <<= 1;
        command <<= 1;
        if (bit & (1 << SD_VOICE_SLOT_COUNT))
            bit = 1;
        slot++;
        tries++;
        slot &= SD_VOICE_SLOT_COUNT - 1;
    } while (tries < SD_VOICE_SLOT_COUNT);
}

void SD_SEStop(s32 id)
{
    u16 key = id;
    s32 value = id;
    s32 bits;
    s32 bit;
    s32 mask;
    s32 i;

    if ((value & 0x8000) != 0) {
        func_800464F0();
        func_80045114();
        return;
    }
    if ((value & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        u8 *table = (u8 *)g_SDValue->field_044C[0];
        s32 offset = (value & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        s32 bank = 0;

        if ((value & SD_VOICE_LOOKUP_BANK_FLAG) != 0) {
            bank = 1;
        }
        bank *= SD_VOICE_LOOKUP_BANK_BYTE_STRIDE;
        offset += bank;
        key = *(u16 *)(table + offset);
        if (key == SD_PENDING_ENTRY_NONE) {
            return;
        }
    }
    mask = 1;
    bits = 0;
    bit = SD_VOICE_SLOT_MASK_BASE;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_ids[i] == key) {
            bits |= bit;
            g_SDValue->voice_active_mask &= ~mask;
            SD_KeyOffVoice(bit);
        }
        mask <<= 1;
        bit <<= 1;
    }
    if (bits != 0) {
        SpuSetKey(0, bits);
    }
}

void SD_KeyOffVoiceSlots(void)
{
    s32 count = 0;
    s32 total;

    do {
        SpuSetKey(SPU_OFF, SD_VOICE_SLOT_MASK_ALL);
        SpuGetAllKeysStatus((char *)g_SDValue->field_15D8);
        total = g_SDValue->field_15EF + g_SDValue->field_15ED +
                g_SDValue->field_15EE + g_SDValue->field_15EF;
        count++;
    } while (total != 0 && count < 24);
}

s32 func_80047F38(u8 value)
{
    s32 i;
    s32 result = 0;
    s32 mask = 1;
    SDValue *state = g_SDValue;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        if ((state->voice_flags[i] & 0xF) == value)
            result |= mask;
    }
    if (result != 0)
        SpuSetKey(SPU_OFF, result << SD_VOICE_SLOT_KEY_SHIFT);
    return result;
}
