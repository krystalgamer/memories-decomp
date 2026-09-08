#ifndef YUGIOH_GAME_FUNC_8005EBF4_H
#define YUGIOH_GAME_FUNC_8005EBF4_H

#include "../types.h"

/* One entry of the keyframe ring at D_800F5788. The record is treated as an
   opaque 0x28-byte blob here: the evaluator only ever indexes into it by
   channel through `(u8 *)key + k * 8`, so no field layout is asserted. */
typedef struct {
    u8 bytes[0x28];
} Key;

/* One 8-byte channel record. The evaluator reads the first three halfwords as
   the interpolated components; `w` covers the halfword at +6, which the
   keyframe records use as a channel kind and which the current pose leaves
   untouched. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u16 w;
} Coeff;

void func_8005EBF4(Key *cur, s32 k, s32 scale, s32 den, s16 *out);

#endif
