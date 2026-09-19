#ifndef MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H
#define MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H

#include "../types.h"

/* Shared geometry-effect tables. func_8006AF74 consumes D_800915E8,
   Model_UpdateDiscEffect consumes D_80091604 and func_8006CD78 consumes
   D_80091610 and D_800916D4; all three are matching C. */
/* The parameter record Model_UpdateDiscEffect points its `table` at: the rim
   colour, the smallest radius the disc is built with, the level it starts at
   and fades from, and the scale it grows to. The last two are also the
   divisors of the colour ramp. D_80091604 is the only instance, and its last
   halfword is zero and unread. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 pad_03;
    s16 min_radius;
    s16 full_level;
    s16 full_scale;
    u16 pad_0A;
} ModelDiscEffectConfig;

extern u32 D_800915E8[];
extern ModelDiscEffectConfig D_80091604;
extern u16 D_80091610[];
extern u32 D_800916D4[];

#endif
