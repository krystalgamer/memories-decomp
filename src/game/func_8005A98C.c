#include "../types.h"
#include "color_constants.h"

/* RGB to HSL-style triple: the second output is lightness from the channel
 * extrema and the third is saturation. Hue wraps at 0x6000. */
typedef struct {
    s32 h;
    s16 s;
    s16 v;
} HsvT;

HsvT *func_8005A98C(HsvT *out, s8 r, u8 g, s8 b, u8 lim) {
    u8 c[3];
    HsvT t;
    u8 x;
    s32 j1;
    s32 j2;
    s32 i;
    s32 hi;
    s32 lo;
    s32 mx;
    s32 mn;
    s32 d2;
    s32 sum;
    s32 q;
    s32 df;
    s32 sh;
    s32 n;

    s32 u;
    hi = 0;
    lo = 0;
    i = 1;
    c[0] = r;
    c[1] = g;
    c[2] = b;

    do {
        x = c[i];
        if (x > c[hi]) {
            hi = i;
        }
        if (x < c[lo]) {
            lo = i;
        }
        i++;
    } while (i < 3);

    mx = c[hi];
    mn = c[lo];
    sum = mx + mn;
    sh = sum << COLOR_FIXED_SHIFT;
    d2 = (lim & 0xFF) * 2;
    q = sh / d2;

    t.v = 0;
    t.h = 0;
    df = mn;
    df = mx - df;
    t.s = q;

    if (df != 0) {
        if ((u32)(q & 0xFFFF) < COLOR_FIXED_HALF + 1) {
            t.v = (df << COLOR_FIXED_SHIFT) / sum;
        } else {
            t.v = (df << COLOR_FIXED_SHIFT) / (d2 - sum);
        }
        j1 = (hi + 1) % 3;
        j2 = (hi + 2) % 3;
        n = (c[j1] - c[j2]) << COLOR_FIXED_SHIFT;
        u = (hi << (COLOR_FIXED_SHIFT + 1)) + n / df;
        n = u;
        t.h = n;
        if (n < 0) {
            t.h = n + COLOR_HUE_FULL_TURN;
        }
    }

    *out = t;
    return out;
}
