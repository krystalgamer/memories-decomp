#ifndef MEMORIES_DECOMP_DUEL_EFFECT_UTILITY_HELPERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_UTILITY_HELPERS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

void func_8014F010(u8 *color, u8 value);
void func_8014F020(u8 *color, u8 red, u8 green, u8 blue);
void func_8014F030(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
void func_8014F180(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
void func_8014F2D4(SVECTOR *vertices, SVECTOR *offset);
void func_8014F358(SVECTOR *vertices, u16 size);
void func_8014F3E8(SVECTOR *vertices, u16 width, u16 height, u16 depth);
s32 func_8014F524(s16 base, u16 exponent);
void func_8014F564(u16 *output, GsIMAGE *image, s32 mode);
void func_8014F5D0(u16 *output, GsIMAGE *image);

#endif
