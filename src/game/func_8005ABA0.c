#include "../types.h"

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

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

    m = (h / 4096 + 1) % 6;
    i0 = m / 2;
    if (m & 1) {
        jb = (i0 + 1) % 3;
        ja = (i0 + 2) % 3;
    } else {
        ja = (i0 + 1) % 3;
        jb = (i0 + 2) % 3;
    }

    if (s < 0x801) {
        ch[ja] = (s * (0x1000 - v) / 4096) * lim;
        ch[i0] = (s << 1) * lim - ch[ja];
    } else {
        ch[i0] = (s * (0x1000 - v) / 4096 + v) * lim;
        ch[ja] = (s << 1) * lim - ch[i0];
    }

    x = (h + 0x1000) % 0x6000 - i0 * 0x2000;
    w = x - 0x1000;
    if (w < 0) {
        w = 0x1000 - x;
    }
    ch[jb] = (ch[i0] - ch[ja]) * w / 4096 + ch[ja];

    ch[0] = (ch[0] + 0x800) / 4096;
    ch[1] = (ch[1] + 0x800) / 4096;
    ch[2] = (ch[2] + 0x800) / 4096;
    t.r = ch[0] < 0x100 ? ch[0] : 0xFF;
    t.g = ch[1] < 0x100 ? ch[1] : 0xFF;
    t.b = ch[2] < 0x100 ? ch[2] : 0xFF;
    *out = t;
    return out;
}
