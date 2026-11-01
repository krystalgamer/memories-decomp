#ifndef MEMORIES_DECOMP_COLOR_H
#define MEMORIES_DECOMP_COLOR_H

#include "../types.h"
#include "../ygo_types.h"
#include "color_constants.h"

HsvT *Color_RgbToHsl(HsvT *out, u8 r, u8 g, u8 b, u8 lim);
Color *Color_HslToRgb(Color *out, s32 h, u16 s, u16 v, u8 lim);

/* Tint a packed BGR555 pixel, preserving its STP bit. Returns 0 unchanged. */
s32 Color_TintBgr555Pixel(u16 color, s32 flags, u16 scale);

/* Hue/lightness/saturation straight to a packed BGR555 pixel, no STP bit. */
s32 Color_HslToBgr555Pixel(s32 value, u32 a, u32 b);

/* Tint an unpacked colour triple; the triple form of Color_TintBgr555Pixel. */
Color *Color_TintRgb(
    Color *out, u8 r, u8 g, u8 b, s32 flags, u16 scale, u8 lim);

#endif
