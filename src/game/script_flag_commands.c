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
    u8 *cursor = D_8009B290;
    u8 *next = cursor + 2;
    u16 command;

    D_8009B290 = next;
    command = cursor[0] | (cursor[1] << 8);
    if (command & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(command & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
    } else {
        u32 offset;

        D_8009B290 = cursor + 4;
        offset = cursor[2] | (next[1] << 8);
        if (Campaign_TestStoryFlag(command))
            D_8009B290 = D_801A8000 + offset;
    }
    D_8009B27C = 0;
}

void Script_OpViewportTween(void)
{
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B2A8 = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B2AA = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B29C = cursor[0] | (cursor[1] << 8);
    }
    D_8009B27C = 7;
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
        n = *(s16 *)&D_8009B29C;
        D_8009B294 = ((*(s16 *)&D_8009B2A8 - sx) << 16) / n;
        sy = *(s16 *)&gGraphics_uViewportY[0];
        D_8009B298 = ((*(s16 *)&D_8009B2AA - sy) << 16) / n;
        D_8009B284 = (sx << 16) | 0x8000;
        D_8009B288 = (sy << 16) | 0x8000;
    }

    a = D_8009B284;
    do { b = D_8009B294; } while (0);
    c = D_8009B288;
    d = D_8009B298;
    a = a + b;
    D_8009B284 = a;
    gGraphics_uViewportX[0] = a >> 16;
    c = c + d;
    D_8009B288 = c;
    gGraphics_uViewportY[0] = c >> 16;

    v = D_8009B29C - 1;
    D_8009B29C = v;
    if ((s16)v <= 0) {
        D_8009B27C = 0;
        gGraphics_uViewportX[0] = D_8009B2A8;
        gGraphics_uViewportY[0] = D_8009B2AA;
    }
}
