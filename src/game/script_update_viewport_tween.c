#include "../types.h"
#include "graphics_frame.h"
#include "script_state.h"
#include "../unmatched.h"
#include "script_command_busy.h"
#include "script_flag_commands.h"

extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));

void Script_UpdateViewportTween(void)
{
    s32 sx;
    s32 sy;
    s32 n;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    if (func_8002E3B4() == 0) {
        sx = gGraphics_sViewportX;
        n = *(s16 *)&D_8009B29C;
        D_8009B294 = ((*(s16 *)&D_8009B2A8 - sx) << 16) / n;
        sy = gGraphics_sViewportY;
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
    gGraphics_sViewportX = a >> 16;
    c = c + d;
    D_8009B288 = c;
    gGraphics_sViewportY = c >> 16;

    v = D_8009B29C - 1;
    D_8009B29C = v;
    if ((s16)v <= 0) {
        D_8009B27C = 0;
        gGraphics_sViewportX = D_8009B2A8;
        gGraphics_sViewportY = D_8009B2AA;
    }
}
