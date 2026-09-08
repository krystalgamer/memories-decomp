#ifndef MEMORIES_DECOMP_COLOR_H
#define MEMORIES_DECOMP_COLOR_H

#include "../types.h"
#include "color_constants.h"

/* An unpacked colour triple, one byte per channel.
 *
 * Every function here works in the 5-bit range the framebuffer uses, so the
 * stored values are 0..COLOR_BGR555_CHANNEL_MASK rather than 0..255, except in
 * func_8005ABA0, which clamps to 0xFF before storing and so is the only place
 * a full byte can appear.
 */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

/* The intermediate triple used by the tint pipeline.
 *
 * `h` is a hue angle in COLOR_FIXED_SHIFT fixed point that wraps at
 * COLOR_HUE_FULL_TURN, so the six hue sectors sit at multiples of
 * COLOR_FIXED_ONE. `s` and `v` are also fixed point, spanning 0..
 * COLOR_FIXED_ONE.
 *
 * The field names follow HSV, but func_8005A98C computes `s` from the mean of
 * the channel extrema and `v` from their spread, which is the HSL lightness
 * and saturation pair rather than an HSV saturation and value. Both halves of
 * the round trip agree on that reading: func_8005ABA0 branches on `s` against
 * COLOR_FIXED_HALF, which is the HSL lightness split. The names are kept as
 * they are because they are what the rest of the tree already uses.
 */
typedef struct {
    s32 h;
    u16 s;
    u16 v;
} HsvT;

/* func_8005A98C and func_8005ABA0 are deliberately absent.
 *
 * Both return their `out` argument, but no caller uses the result and
 * func_8005B0B4 declares them as returning void. That is not cosmetic: giving
 * that file the real return types leaves the call sites semantically identical
 * yet shifts eight words of register allocation in its tail, because the
 * discarded return value keeps v0 live across the calls. The declarations
 * therefore stay with each caller until that divergence is understood.
 */

/* Tint a packed BGR555 pixel, preserving its STP bit. Returns 0 unchanged. */
s32 func_8005AE68(u16 color, s32 flags, u16 scale);

/* Hue/lightness/saturation straight to a packed BGR555 pixel, no STP bit. */
s32 func_8005B054(s32 value, u32 a, u32 b);

/* Tint an unpacked colour triple; the triple form of func_8005AE68. */
Color *func_8005B0B4(
    Color *out, u8 r, u8 g, u8 b, s32 flags, u16 scale, u8 lim);

#endif
