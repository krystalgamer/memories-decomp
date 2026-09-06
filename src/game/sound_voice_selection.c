#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef float f32;
typedef double f64;
typedef char M2C_UNK;
typedef struct { s32 words[9]; } Mtx32;
typedef u8 State;
typedef u8 Record;
typedef u8 Pair;
typedef u8 Controller;
typedef u8 Entry;
typedef u8 Rec;
typedef u8 Block;
typedef struct { u32 words[2]; } Blk8;

extern void func_80047788(u16);
extern void func_8004763C(void);

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
        *(s32 *)(state + 0x3C8) = 3;
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
            SpuSetKey(0, mask);
            accum |= mask;
        }
    tail:
        timer = g_SDValue->voice_timer[i];
        if (timer != 0) {
            timer -= 1;
            g_SDValue->voice_timer[i] = timer;
            if (timer == 0) {
                v0 = SpuGetKeyStatus(mask);
                if (v0 != 0) {
                    SpuSetKey(0, mask);
                    accum |= mask;
                }
            }
        }
        v0 = SpuGetKeyStatus(mask);
        if (v0 == 3 && !(accum & mask)) {
            SpuSetKey(0, mask);
            accum |= mask;
        }
        bit2 <<= 1;
        mask <<= 1;
    }
    if (accum != 0)
        SpuSetKey(0, accum);
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
    SpuSetKey(0, result);
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

    SpuSetKey(0, SD_VOICE_SLOT_MASK_ALL);
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
        SpuSetKey(0, value);
        if (SpuGetKeyStatus(value) == 0)
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
    s32 command = 1 << ((u8)slot + 20);

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
