#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B27C-0x8009B280, defined in
 * address order ahead of the headers that declare it (#2602). */
u16 gScript_wCommandWord;
static u16 D_8009B27E;

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
