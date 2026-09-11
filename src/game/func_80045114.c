#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

void func_80045114(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->flags_004A & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands.c[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands.c[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands.c[count - 2].command == 0x11)
                return;
        }
    }
    func_8004503C(-0x20, 0, 1);
    {
        SDCommand local;
        local.command = 0x11;
        SD_EnqueueCommand(&local);
    }
}

s16 func_800451E0(u16 value, s32 unused)
{
    return func_80045208(value, 0x80);
}

#include "sound_buffer_init.h"

