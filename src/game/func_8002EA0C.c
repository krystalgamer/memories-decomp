#include "../types.h"

extern u16 D_8009B146[4];
extern u16 D_8009B148[4];
extern s16 D_8009B27C;
extern s32 D_8009B284;
extern s32 D_8009B288;
extern s32 D_8009B294;
extern s32 D_8009B298;
extern u16 D_8009B29C;
extern s16 D_8009B2A8;
extern s16 D_8009B2AA;

s32 func_8002E3B4(void);

/* Two-axis smooth scroll stepper: on the first frame derives the per-frame
 * 16.16 deltas from the distance to the target over the remaining frame
 * count, then advances both accumulators, publishes their high halves as
 * the camera position, and snaps to the target when the counter runs out. */
void func_8002EA0C(void) {
    s32 sx;
    s32 sy;
    s32 n;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    if (func_8002E3B4() == 0) {
        sx = *(s16 *)&D_8009B146[0];
        n = *(s16 *)&D_8009B29C;
        D_8009B294 = ((*(s16 *)&D_8009B2A8 - sx) << 16) / n;
        sy = *(s16 *)&D_8009B148[0];
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
    D_8009B146[0] = a >> 16;
    c = c + d;
    D_8009B288 = c;
    D_8009B148[0] = c >> 16;

    v = D_8009B29C - 1;
    D_8009B29C = v;
    if ((s16)v <= 0) {
        D_8009B27C = 0;
        D_8009B146[0] = D_8009B2A8;
        D_8009B148[0] = D_8009B2AA;
    }
}
