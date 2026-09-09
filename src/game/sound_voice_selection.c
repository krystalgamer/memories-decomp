#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

extern void func_80047788(u16);

void func_80047864(s32 index)
{
    register s32 saved asm("$5");

    asm volatile("move %0,%1" : "=r"(saved) : "r"(index));
    {
        register u32 mask asm("$3") = SD_VOICE_SLOT_MASK_BASE;
        u8 *state;
        register u8 *half asm("$3");
        register u8 *byte asm("$6");
        u32 product;

        if (saved != 0)
            mask <<= saved;
        state = (u8 *)g_SDValue;
        *(u32 *)(state + 0x3C4) = mask;
        half = state + (saved << 1);
        byte = state + saved;
        *(s32 *)(state + 0x3C8) = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
        {
            register u32 first asm("$5") = *(u16 *)(half + 0x414);
            product = first * (u32)byte[0x424];
        }
        *(u16 *)(state + 0x3CC) = product >> 8;
        product = *(u16 *)(half + 0x41C) * (u32)byte[0x424];
        *(u16 *)(state + 0x3CE) = product >> 8;
        SpuSetVoiceAttr((SpuVoiceAttr *)(state + 0x3C4));
    }
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

s32 func_80047B68(u16 value)
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
    register s32 result asm("$2") = value;

    value &= 0x8000;
    if (value)
        return result & 0xFFFF;
    return 0xFFFF;
}

void func_80047C70(s32 value)
{
    s32 count;

    for (count = 0; count < SD_KEY_OFF_RETRY_LIMIT; count++) {
        SpuSetKey(SPU_OFF, value);
        if (SpuGetKeyStatus(value) == SPU_OFF)
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
                func_80047C70(command);
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

void func_80047DB0(s32 arg)
{
    register s32 value asm("$5") = arg;
    u16 key = value;
    s32 bits;
    s32 bit;
    s32 mask;
    s32 i;

    if (value & 0x8000) {
        func_800464F0();
        func_80045114();
        return;
    }
    if ((value & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        u8 *table = (u8 *)g_SDValue->field_044C[0];
        s32 offset = (value & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        register s32 bank asm("$2") = 0;

        if (value & SD_VOICE_LOOKUP_BANK_FLAG)
            bank = 1;
        bank *= SD_VOICE_LOOKUP_BANK_BYTE_STRIDE;
        offset += bank;
        key = *(u16 *)(table + offset);
        if (key == SD_PENDING_ENTRY_NONE)
            return;
    }
    mask = 1;
    bits = 0;
    bit = SD_VOICE_SLOT_MASK_BASE;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_ids[i] == key) {
            bits |= bit;
            g_SDValue->voice_active_mask &= ~mask;
            func_80047C70(bit);
        }
        mask <<= 1;
        bit <<= 1;
    }
    if (bits != 0)
        SpuSetKey(0, bits);
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
