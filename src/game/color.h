#ifndef MEMORIES_DECOMP_COLOR_H
#define MEMORIES_DECOMP_COLOR_H

#include "../types.h"
#include "../ygo_types.h"
#include "color_constants.h"

HsvT *Color_RgbToHsl(HsvT *out, u8 r, u8 g, u8 b, u8 lim);
Color *Color_HslToRgb(Color *out, s32 h, u16 s, u16 v, u8 lim);

/* Caller-side same-symbol views whose discarded returns and widened arguments
 * are code-generation sensitive. The public declarations above remain the
 * definitions' real types. Only color_transform.c uses the views, so they sit
 * behind COLOR_CALLER_VIEWS: the other consumers of this header are alias-free
 * units and must stay that way after preprocessing. */
#ifdef COLOR_CALLER_VIEWS
extern void Color_RgbToHsl_void(
    HsvT *out, u8 r, u8 g, u8 b, u8 lim
) asm("Color_RgbToHsl");
extern void Color_HslToRgb_wide(
    Color *out, s32 h, u32 s, u32 v, s32 lim
) asm("Color_HslToRgb");
#endif

/* Tint a packed BGR555 pixel, preserving its STP bit. Returns 0 unchanged. */
s32 Color_TintBgr555Pixel(u16 color, s32 flags, u16 scale);

/* Hue/lightness/saturation straight to a packed BGR555 pixel, no STP bit. */
s32 func_8005B054(s32 value, u32 a, u32 b);

/* Tint an unpacked colour triple; the triple form of Color_TintBgr555Pixel. */
Color *func_8005B0B4(
    Color *out, u8 r, u8 g, u8 b, s32 flags, u16 scale, u8 lim);

#endif
