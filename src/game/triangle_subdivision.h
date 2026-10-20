#ifndef MEMORIES_DECOMP_TRIANGLE_SUBDIVISION_H
#define MEMORIES_DECOMP_TRIANGLE_SUBDIVISION_H

#include "../types.h"
#include "../ygo_types.h"

s16 *Model_SubdivideTriangle(s16 *a, s16 *b, s16 *c, s16 *out, s32 depth);
Triplet *Triplet_SubdivideTriangle(u8 *a, u8 *b, u8 *c, Triplet *out, s32 depth);
void Triplet_SetComponents(u8 *output, s32 first, s32 second, s32 third);
void Triplet_CopyComponents(u8 *destination, const u8 *source);
void Triplet_AverageComponents(u8 *out, u8 *a, u8 *b);

#endif
