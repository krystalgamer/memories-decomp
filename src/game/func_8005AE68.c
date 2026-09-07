#include "../types.h"

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

typedef struct {
    s32 h;
    u16 s;
    u16 v;
} HsvT;

extern HsvT *func_8005A98C(HsvT *, s8, u8, s8, u8);
extern Color *func_8005ABA0(Color *, s32, u16, u16, u8);

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

    inverted = flags & 8;
    sector = flags & 7;
    gray = ((u8)sector == 6);
    func_8005A98C(&hsv, color & 31, (color >> 5) & 31, (color >> 10) & 31, 31);

    lim = 31;
    if ((u8)sector < 7) {
        hue = sector;
        if (inverted) {
            hue = (hue + 3) % 6;
        }
        hsv.h = hue << 12;
    }

    if (gray) {
        hsv.v = 0;
    } else {
        hsv.v = (hsv.v * scale) / 4096;
    }

    func_8005ABA0(&out, hsv.h, hsv.s, hsv.v, 31);

    if (inverted) {
        out.r = lim - out.r;
        out.g = lim - out.g;
        out.b = lim - out.b;
    }

    out.r = out.r ? out.r : 1;
    out.g = out.g ? out.g : 1;
    out.b = out.b ? out.b : 1;
    packed = out;
    return (packed.r & 31) | ((packed.g & 31) << 5) | ((packed.b & 31) << 10)
         | (color & 0x8000);
}
