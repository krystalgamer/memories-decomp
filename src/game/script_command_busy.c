#include "../types.h"
#include "script_command_busy.h"
#include "script_state.h"

s32 ScriptCommand_MarkStarted(void)
{
    u16 value = gScript_wCommandWord;

    if (!(value & 0x8000)) {
        gScript_wCommandWord = value | 0x8000;
        return 0;
    }
    return 1;
}

void Script_OpHalt(void)
{
    ScriptCommand_MarkStarted();
}
