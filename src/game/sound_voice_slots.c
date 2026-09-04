#include "../types.h"
#include "sound.h"

extern s32 func_80077090(s32);
extern void func_80076ED0(s32, s32);

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
        if (bit & 0x10)
            bit = 1;
        slot++;
        tries++;
        slot &= 3;
    } while (tries < 4);
}
