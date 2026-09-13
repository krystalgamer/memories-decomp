#include "../types.h"
#include "sound.h"
#include "sound_output.h"
#include "sound_output_state.h"
#include "sound_init.h"

void func_8004733C(s32 arg0, s32 arg1)
{
    u16 saved = arg0;

    if ((g_SDValue->flags_004A & 2) == 0)
        return;

    if (arg0 & 0x8000) {
        func_800473CC(SD_BGM_COMMAND_BASE);
        arg1 = (s16)arg1;
        func_80045208(saved & SD_COMMAND_VALUE_MASK, arg1);
    } else {
        u32 masked = (u32)(saved & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE)
            arg0 -= SD_BGM_COMMAND_BASE;
        arg1 = (s16)arg1;
        func_80049230((s16)arg0, arg1);
    }
}
