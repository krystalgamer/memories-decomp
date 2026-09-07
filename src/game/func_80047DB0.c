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
    if ((value & 0xF000) == 0x4000) {
        u8 *table = (u8 *)g_SDValue->field_044C[0];
        s32 offset = (value & 0x1F) * 2;
        register s32 bank asm("$2") = 0;

        if (value & 0x100)
            bank = 1;
        bank *= 64;
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
