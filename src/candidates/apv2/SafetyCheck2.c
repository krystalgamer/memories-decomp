#include "../../types.h"
#include "apv2.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libetc.h"
#define PSYQ_EXIT_UNPROTOTYPED
#include "../../psyq/exit.h"

extern Apv2Message D_8016947C[];
extern s32 D_801694D0;

extern void SafetyCheck3(u8 *, Apv2Rect *, s32);
extern void SafetyCheck5(void);

void SafetyCheck2(void)
{
    DRAWENV draw;
    DISPENV disp;
    DR_ENV drenv;
    u32 prim[3];
    Apv2Rect rect;
    s32 i;
    s32 color;
    s16 x;
    s16 y;

    ResetGraph(1);
    StopCallback();
    ResetGraph(5);
    *(u16 *)0x1F801DAA = 0;
    SetDefDrawEnv(&draw, 0, 0, 320, 240);
    SetDefDispEnv(&disp, 0, 0, 320, 240);
    draw.isbg = 1;
    SetDrawEnv(&drenv, &draw);
    DrawPrim(&drenv);
    PutDispEnv(&disp);
    prim[0] = 0x02000000;
    prim[1] = 0xE6000002;
    prim[2] = 0;
    DrawPrim(prim);
    color = 0xFFFF;
    rect.w = D_8016947C[D_801694D0].width;
    rect.h = 1;
    for (i = 0; i < 2; i++) {
        x = D_8016947C[D_801694D0].x;
        y = D_8016947C[D_801694D0].y;
        rect.x = x + i;
        rect.y = y + i;
        SafetyCheck3(D_8016947C[D_801694D0].text, &rect, color);
        color = 0x8000;
    }
    SafetyCheck5();
    SetDispMask(1);
    exit();
}
