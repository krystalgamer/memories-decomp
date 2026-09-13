#include "../types.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_output.h"

void func_800472A8(s32 arg0)
{
    u16 saved = arg0;

    if ((g_SDValue->flags_004A & 2) == 0)
        return;

    if (arg0 & 0x8000) {
        func_80045334(saved & SD_COMMAND_VALUE_MASK);
    } else {
        u32 masked = (u32)(saved & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE)
            arg0 -= SD_BGM_COMMAND_BASE;
        func_80049138((s16)arg0, 1);
    }
}
