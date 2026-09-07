#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

void SD_KeyOffVoiceSlots(void)
{
    s32 count = 0;
    s32 total;

    do {
        SpuSetKey(0, SD_VOICE_SLOT_MASK_ALL);
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
        SpuSetKey(0, result << SD_VOICE_SLOT_KEY_SHIFT);
    return result;
}
