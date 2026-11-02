#include "../types.h"
#include "script_command_busy.h"
#include "fade.h"
#include "script_state.h"
#include "script_op_fade_out.h"
#include "../unmatched.h"

void Script_OpFadeOut(void)
{
    int command;

    if (ScriptCommand_MarkStarted() == 0) {
        command = *gScript_StreamCursor++;
        Fade_InitOut();
        if (command & 0x3F) {
            gFade_State.step = command & 0x3F;
        }
        if ((command & 0x80) == 0) {
            goto clear;
        }
        goto end;
    }
    if (gFade_State.flags & 0x80) {
        goto end;
    }
clear:
    gScript_wCommandWord = 0;
end:;
}
