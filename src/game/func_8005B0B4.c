#define FUNC_8005A98C_RETURNS_VOID
#define FUNC_8005ABA0_WIDE_VOID
#include "../types.h"
#include "color.h"

Color *func_8005B0B4(Color *out, u8 r, u8 g, u8 b, s32 flags, u16 scale, u8 lim) {
    HsvT hsv;
    Color c;
    s32 idx;
    s32 inv;
    s32 flat;
    u8 k;

    inv = flags & COLOR_TINT_INVERT;
    idx = flags & COLOR_TINT_HUE_MASK;
    flat = ((u8)idx == COLOR_TINT_GRAYSCALE);

    func_8005A98C(&hsv, r, g, b, lim);

    if ((u8)idx < COLOR_TINT_KEEP_HUE) {
        k = idx;
        if (inv) {
            k = (k + COLOR_HUE_SECTOR_COUNT / 2) % COLOR_HUE_SECTOR_COUNT;
        }
        hsv.h = k << COLOR_FIXED_SHIFT;
    }

    if (flat) {
        hsv.v = 0;
    } else {
        hsv.v = hsv.v * scale / COLOR_FIXED_ONE;
    }

    func_8005ABA0(&c, hsv.h, hsv.s, hsv.v, lim);

    if (inv) {
        c.r = lim - c.r;
        c.g = lim - c.g;
        c.b = lim - c.b;
    }

    c.r = c.r ? c.r : 1;
    c.g = c.g ? c.g : 1;
    c.b = c.b ? c.b : 1;

    *out = c;
    return out;
}
