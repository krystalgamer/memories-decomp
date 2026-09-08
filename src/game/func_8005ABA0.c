#include "../types.h"
#include "color.h"

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
