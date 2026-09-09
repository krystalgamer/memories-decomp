#include "../types.h"
#define D_8009B142_IN_DATA
#include "graphics_frame.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libcd.h"
#include "graphics_constants.h"

extern u8 *D_80010000 __attribute__((section(".data")));

void func_8005C690(void);

s32 func_8005B8A0(u8 *src, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    RECT rect;
    s32 m;
    s32 r;

    D_8009B063 = 0;
    D_8009B064 = 0;
    D_8009B065 = 0xFF;
    D_8009B070 = a3;
    D_8009B060 = a4;
    D_8009B498 = D_80010000;
    D_8009B061 = a5;
    if (D_8009B060 != 0) {
        /* Retail sets $a0 and $a1 here even though GsGetActiveBuff takes
           no arguments; the two values are kept so the call setup matches. */
        m = GsGetActiveBuff(D_80010000, a5);
        rect.x = m * 0x140;
        rect.y = 0;
        rect.w = D_800FE0D0;
        rect.h = D_800FE0D4;
        ClearImage(&rect, D_8009B144, D_8009B143, D_8009B142);
        DrawSync(0);
        VSync(0);
        GsSwapDispBuff();
        rect.x = (m ^ 1) * 0x140;
        rect.w = D_800FE0D0;
        rect.y = 0;
        rect.h = D_800FE0D4;
        ClearImage(&rect, D_8009B144, D_8009B143, D_8009B142);
        rect.x = 0;
        rect.y = 0x100;
        rect.w = *(s32 *)&D_800FE0D0 * 0x1800 / 4096;
        rect.h = D_800FE0D4;
        ClearImage(&rect, D_8009B144, D_8009B143, D_8009B142);
        DrawSync(0);
        VSync(0);
        D_800FE0CC = 1;
        GsSwapDispBuff();
        GsDefDispBuff(0, 0, 0, 0x100);
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 1);
    }
    if (src != (u8 *)0) {
        D_8009B49C = *(CdlLOC *)src;
    }
    if (a1 != 0) {
        D_8009B068 = a1;
    } else {
        D_8009B068 = 1;
    }
    if (a2 != 0) {
        D_8009B06C = a2;
    } else {
        D_8009B06C = 0xFFFF;
    }
    D_8009B066 = 0;
    D_8009B067 = 0;
    DecDCTReset(0);
    DecDCToutCallback(func_8005C1F4);
    DecDCTvlcBuild(D_8009B498);
    StSetRing((u32 *)(D_8009B498 + 0x11000), 0x14);
    StClearRing();
    StSetStream(D_8009B060, D_8009B068, D_8009B06C, 0, func_8005C690);
    if (D_8009B06C >= 5) {
        D_8009B06C = D_8009B06C - 4;
    }
    func_8005C62C(&D_8009B49C);
    r = func_8005BFC8(0);
    if (r != 0) {
        return r;
    }
    return 0;
}
