#include "../types.h"
#include "color.h"

HsvT *func_8005A98C(HsvT *out, u8 r, u8 g, u8 b, u8 lim) {
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

Color *func_8005ABA0(Color *out, s32 h, u16 s, u16 v, u8 lim)
{
    u32 ch[3];
    Color t;
    s32 m;
    s32 i0;
    s32 ja;
    s32 jb;
    s32 w;
    s32 x;

    m = (h / COLOR_FIXED_ONE + 1) % COLOR_HUE_SECTOR_COUNT;
    i0 = m / 2;
    if (m & 1) {
        jb = (i0 + 1) % 3;
        ja = (i0 + 2) % 3;
    } else {
        ja = (i0 + 1) % 3;
        jb = (i0 + 2) % 3;
    }

    if (s < COLOR_FIXED_HALF + 1) {
        ch[ja] = (s * (COLOR_FIXED_ONE - v) / COLOR_FIXED_ONE) * lim;
        ch[i0] = (s << 1) * lim - ch[ja];
    } else {
        ch[i0] = (s * (COLOR_FIXED_ONE - v) / COLOR_FIXED_ONE + v) * lim;
        ch[ja] = (s << 1) * lim - ch[i0];
    }

    x = (h + COLOR_FIXED_ONE) % COLOR_HUE_FULL_TURN -
        i0 * (2 * COLOR_FIXED_ONE);
    w = x - COLOR_FIXED_ONE;
    if (w < 0) {
        w = COLOR_FIXED_ONE - x;
    }
    ch[jb] = (ch[i0] - ch[ja]) * w / COLOR_FIXED_ONE + ch[ja];

    ch[0] = (ch[0] + COLOR_FIXED_HALF) / COLOR_FIXED_ONE;
    ch[1] = (ch[1] + COLOR_FIXED_HALF) / COLOR_FIXED_ONE;
    ch[2] = (ch[2] + COLOR_FIXED_HALF) / COLOR_FIXED_ONE;
    t.r = ch[0] < 0x100 ? ch[0] : 0xFF;
    t.g = ch[1] < 0x100 ? ch[1] : 0xFF;
    t.b = ch[2] < 0x100 ? ch[2] : 0xFF;
    *out = t;
    return out;
}
s32 func_8005AE68(u16 color, s32 flags, u16 scale)
{
    Color packed;
    HsvT hsv;
    Color out;
    u8 lim;
    s32 sector;
    u8 hue;
    s32 inverted;
    s32 gray;

    if (color == 0) {
        return 0;
    }

    inverted = flags & COLOR_TINT_INVERT;
    sector = flags & COLOR_TINT_HUE_MASK;
    gray = ((u8)sector == COLOR_TINT_GRAYSCALE);
    func_8005A98C(
        &hsv,
        color & COLOR_BGR555_CHANNEL_MASK,
        (color >> COLOR_BGR555_GREEN_SHIFT) & COLOR_BGR555_CHANNEL_MASK,
        (color >> COLOR_BGR555_BLUE_SHIFT) & COLOR_BGR555_CHANNEL_MASK,
        COLOR_BGR555_CHANNEL_MASK);

    lim = COLOR_BGR555_CHANNEL_MASK;
    if ((u8)sector < COLOR_TINT_KEEP_HUE) {
        hue = sector;
        if (inverted) {
            hue = (hue + COLOR_HUE_SECTOR_COUNT / 2) % COLOR_HUE_SECTOR_COUNT;
        }
        hsv.h = hue << COLOR_FIXED_SHIFT;
    }

    if (gray) {
        hsv.v = 0;
    } else {
        hsv.v = (hsv.v * scale) / COLOR_FIXED_ONE;
    }

    func_8005ABA0(&out, hsv.h, hsv.s, hsv.v, COLOR_BGR555_CHANNEL_MASK);

    if (inverted) {
        out.r = lim - out.r;
        out.g = lim - out.g;
        out.b = lim - out.b;
    }

    out.r = out.r ? out.r : 1;
    out.g = out.g ? out.g : 1;
    out.b = out.b ? out.b : 1;
    packed = out;
    return (packed.r & COLOR_BGR555_CHANNEL_MASK) |
           ((packed.g & COLOR_BGR555_CHANNEL_MASK) << COLOR_BGR555_GREEN_SHIFT) |
           ((packed.b & COLOR_BGR555_CHANNEL_MASK) << COLOR_BGR555_BLUE_SHIFT) |
           (color & COLOR_BGR555_STP_MASK);
}

/* These names bind to the definitions above while preserving the caller-side
 * contracts that produce the retail allocation in the final two functions. */
extern void func_8005A98C_void(
    HsvT *out, u8 r, u8 g, u8 b, u8 lim
) asm("func_8005A98C");
extern void func_8005ABA0_wide(
    Color *out, s32 h, u32 s, u32 v, s32 lim
) asm("func_8005ABA0");

s32 func_8005B054(s32 value, u32 a, u32 b)
{
    Color color;

    func_8005ABA0_wide(
        &color, value, a & 0xFFFF, b & 0xFFFF, COLOR_BGR555_CHANNEL_MASK
    );
    return (color.r & COLOR_BGR555_CHANNEL_MASK) |
           ((color.g & COLOR_BGR555_CHANNEL_MASK)
            << COLOR_BGR555_GREEN_SHIFT) |
           ((color.b & COLOR_BGR555_CHANNEL_MASK)
            << COLOR_BGR555_BLUE_SHIFT);
}

Color *func_8005B0B4(
    Color *out, u8 r, u8 g, u8 b, s32 flags, u16 scale, u8 lim
)
{
    HsvT hsv;
    Color c;
    s32 idx;
    s32 inv;
    s32 flat;
    u8 k;

    inv = flags & COLOR_TINT_INVERT;
    idx = flags & COLOR_TINT_HUE_MASK;
    flat = ((u8)idx == COLOR_TINT_GRAYSCALE);

    func_8005A98C_void(&hsv, r, g, b, lim);

    if ((u8)idx < COLOR_TINT_KEEP_HUE) {
        k = idx;
        if (inv) {
            k = (k + COLOR_HUE_SECTOR_COUNT / 2) %
                COLOR_HUE_SECTOR_COUNT;
        }
        hsv.h = k << COLOR_FIXED_SHIFT;
    }

    if (flat) {
        hsv.v = 0;
    } else {
        hsv.v = hsv.v * scale / COLOR_FIXED_ONE;
    }

    func_8005ABA0_wide(&c, hsv.h, hsv.s, hsv.v, lim);

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
