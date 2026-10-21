#ifndef MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H
#define MEMORIES_DECOMP_MODEL_GEOMETRY_TABLES_H

#include "../types.h"

/* Shared geometry-effect tables. Model_UpdateSubdividedEffect consumes gModel_SubdividedEffectConfigWords,
   Model_UpdateDiscEffect consumes gModel_DiscEffectConfig and Model_UpdateBurstEffect consumes
   gModel_BurstImageData and gModel_BurstRingPaletteWords; all three are matching C. */
/* The parameter record Model_UpdateDiscEffect points its `table` at: the rim
   colour, the smallest radius the disc is built with, the level it starts at
   and fades from, and the scale it grows to. The last two are also the
   divisors of the colour ramp. gModel_DiscEffectConfig is the only instance, and its last
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

extern u32 gModel_SubdividedEffectConfigWords[];
extern ModelDiscEffectConfig gModel_DiscEffectConfig;
extern u16 gModel_BurstImageData[];
extern u32 gModel_BurstRingPaletteWords[];

#endif
