#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

extern void func_800464F0(void);
extern void func_80045114(void);
extern void func_80047C70(s32);

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
        if (key == 0xFFFF)
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
