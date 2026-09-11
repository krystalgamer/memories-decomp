#include "../types.h"
#include "sound_voice_selection.h"

s32 func_80047C50(s32 value)
{
    u16 result = value;

    value &= 0x8000;
    if (value) {
        return result;
    }
    return 0xFFFF;
}
