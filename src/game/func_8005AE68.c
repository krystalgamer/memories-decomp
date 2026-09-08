#include "../types.h"
#include "color.h"

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
