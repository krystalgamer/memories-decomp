#define GGRAPHICS_VIEWPORT_SIZED_UNSIGNED_IN_DATA
#include "../types.h"
#include "campaign_flags.h"
#include "graphics_frame.h"
#include "script_command_busy.h"
#include "script_state.h"
#include "script_flag_commands.h"
#include "../unmatched.h"

void Script_OpStoryFlag(void)
{
    u8 *cursor = gScript_StreamCursor;
    u8 *next = cursor + 2;
    u16 command;

    gScript_StreamCursor = next;
    command = cursor[0] | (cursor[1] << 8);
    if (command & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(command & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
    } else {
        u32 offset;

        gScript_StreamCursor = cursor + 4;
        offset = cursor[2] | (next[1] << 8);
        if (Campaign_TestStoryFlag(command))
            gScript_StreamCursor = D_801A8000 + offset;
    }
    gScript_wCommandWord = 0;
}

void Script_OpViewportTween(void)
{
    {
        u8 *cursor = gScript_StreamCursor;

        gScript_StreamCursor = cursor + 2;
        gScript_ViewportTargetX = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = gScript_StreamCursor;

        gScript_StreamCursor = cursor + 2;
        gScript_ViewportTargetY = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = gScript_StreamCursor;

        gScript_StreamCursor = cursor + 2;
        gScript_SharedOperand = cursor[0] | (cursor[1] << 8);
    }
    gScript_wCommandWord = 7;
}

void Script_UpdateViewportTween(void) {
    s32 sx;
    s32 sy;
    s32 n;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    if (ScriptCommand_MarkStarted() == 0) {
        sx = *(s16 *)&gGraphics_uViewportX[0];
        n = *(s16 *)&gScript_SharedOperand;
        gScript_ViewportDeltaX = ((*(s16 *)&gScript_ViewportTargetX - sx) << 16) / n;
        sy = *(s16 *)&gGraphics_uViewportY[0];
        gScript_ViewportDeltaY = ((*(s16 *)&gScript_ViewportTargetY - sy) << 16) / n;
        gScript_ViewportAccumulatorX = (sx << 16) | 0x8000;
        gScript_ViewportAccumulatorY = (sy << 16) | 0x8000;
    }

    a = gScript_ViewportAccumulatorX;
    do { b = gScript_ViewportDeltaX; } while (0);
    c = gScript_ViewportAccumulatorY;
    d = gScript_ViewportDeltaY;
    a = a + b;
    gScript_ViewportAccumulatorX = a;
    gGraphics_uViewportX[0] = a >> 16;
    c = c + d;
    gScript_ViewportAccumulatorY = c;
    gGraphics_uViewportY[0] = c >> 16;

    v = gScript_SharedOperand - 1;
    gScript_SharedOperand = v;
    if ((s16)v <= 0) {
        gScript_wCommandWord = 0;
        gGraphics_uViewportX[0] = gScript_ViewportTargetX;
        gGraphics_uViewportY[0] = gScript_ViewportTargetY;
    }
}
