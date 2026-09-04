#include "../types.h"
#include "sound.h"

extern s32 func_80045208(u16, s32);
extern void func_8004503C(s16, s32, s32);
extern void func_80045BE8(u8 *);

void func_80045114(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->flags_004A & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands[count - 2].command == 0x11)
                return;
        }
    }
    func_8004503C(-0x20, 0, 1);
    {
        u8 local[SD_COMMAND_RECORD_SIZE];
        local[0] = 0x11;
        func_80045BE8(local);
    }
}

s16 func_800451E0(u16 value)
{
    return func_80045208(value, 0x80);
}
