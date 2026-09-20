#ifndef MEMORIES_DECOMP_TRIANGLE_SUBDIVISION_H
#define MEMORIES_DECOMP_TRIANGLE_SUBDIVISION_H

#include "../types.h"
#include "../ygo_types.h"

s16 *func_8006BCA4(s16 *a, s16 *b, s16 *c, s16 *out, s32 depth);
Triplet *func_8006C120(u8 *a, u8 *b, u8 *c, Triplet *out, s32 depth);
void Triplet_SetComponents(u8 *output, s32 first, s32 second, s32 third);
void func_8006C30C(u8 *destination, const u8 *source);
void func_8006C330(u8 *out, u8 *a, u8 *b);

#endif
