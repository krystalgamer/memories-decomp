#include "../types.h"
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

extern s32 func_80047864();
extern void func_80047788(u16);
extern void func_8004763C(void);
extern void func_80076ED0(s32, s32);
extern s32 func_80077090(s32);

void func_800478EC(void)
{
    s32 mask;
    s32 bit2;
    s32 accum;
    s32 i;
    s32 v0;
    u16 timer;

    mask = 0x100000;
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
            func_80076ED0(0, mask);
            accum |= mask;
        }
    tail:
        timer = g_SDValue->voice_timer[i];
        if (timer != 0) {
            timer -= 1;
            g_SDValue->voice_timer[i] = timer;
            if (timer == 0) {
                v0 = func_80077090(mask);
                if (v0 != 0) {
                    func_80076ED0(0, mask);
                    accum |= mask;
                }
            }
        }
        v0 = func_80077090(mask);
        if (v0 == 3 && !(accum & mask)) {
            func_80076ED0(0, mask);
            accum |= mask;
        }
        bit2 <<= 1;
        mask <<= 1;
    }
    if (accum != 0)
        func_80076ED0(0, accum);
}

void func_80047A68(void)
{
    s32 i;
    u32 mask = 0x100000;
    u32 result = 0;
    u16 threshold = g_SDValue->field_0004;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_ids[i] >= threshold)
            result |= mask;
        mask <<= 1;
    }
    func_80076ED0(0, result);
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

    func_80076ED0(0, 0x00F00000);
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

    for (count = 0; count < 256; count++) {
        func_80076ED0(0, value);
        if (func_80077090(value) == 0)
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
