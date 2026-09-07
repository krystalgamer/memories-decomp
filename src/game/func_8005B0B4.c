#include "../types.h"

typedef struct {
    s32 h;
    u16 s;
    u16 v;
} HsvT;

typedef struct {
    u8 r, g, b;
} Color;

extern void func_8005A98C(HsvT *, u8, u8, u8, u8);
extern void func_8005ABA0(Color *, s32, u32, u32, s32);

Color *func_8005B0B4(Color *out, u8 r, u8 g, u8 b, s32 flags, u16 scale, u8 lim) {
    HsvT hsv;
    Color c;
    s32 idx;
    s32 inv;
    s32 flat;
    u8 k;
    u8 tr;
    u8 tg;
    u8 tb;

    inv = flags & 8;
    idx = flags & 7;
    flat = ((u8)idx == 6);

    func_8005A98C(&hsv, r, g, b, lim);

    if ((u8)idx < 7) {
        k = idx;
        if (inv) {
            k = (k + 3) % 6;
        }
        hsv.h = k << 12;
    }

    if (flat) {
        hsv.v = 0;
    } else {
        hsv.v = hsv.v * scale / 4096;
    }

    func_8005ABA0(&c, hsv.h, hsv.s, hsv.v, lim);

    if (inv) {
        c.r = lim - c.r;
        c.g = lim - c.g;
        c.b = lim - c.b;
    }

    tr = c.r;
    if (tr == 0) {
        tr = 1;
    }
    c.r = tr;
    tg = c.g;
    if (tg == 0) {
        tg = 1;
    }
    c.g = tg;
    tb = c.b;
    if (tb == 0) {
        tb = 1;
    }
    c.b = tb;

    *out = c;
    return out;
}
