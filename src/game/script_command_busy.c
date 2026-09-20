#include "../types.h"
#include "script_command_busy.h"
#include "script_state.h"

s32 ScriptCommand_MarkStarted(void)
{
    u16 value = D_8009B27C;

    if (!(value & 0x8000)) {
        D_8009B27C = value | 0x8000;
        return 0;
    }
    return 1;
}

void Script_OpHalt(void)
{
    ScriptCommand_MarkStarted();
}
