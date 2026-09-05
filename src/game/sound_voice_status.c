#include "../types.h"
#include "sound.h"

extern void SpuSetKey(long, unsigned long);
extern void func_800773C4(void *);

void func_80047EC4(void)
{
    s32 count = 0;
    s32 total;

    do {
        SpuSetKey(0, 0x00F00000);
        func_800773C4(g_SDValue->field_15D8);
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
        SpuSetKey(0, result << 20);
    return result;
}
