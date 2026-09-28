#ifndef MEMORIES_DECOMP_DUEL_EFFECT_DRAWING_HELPERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_DRAWING_HELPERS_H

#include "../../types.h"
#include "utility_helpers.h"

extern GsOT *D_8015B7F4;
extern s16 D_8015B7F8[2];
extern u16 D_8015B800;

void func_8015131C(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode);
void func_80152EC4(POLY_G4 *packet, u16 mode);
void func_80152F9C(POLY_FT4 *packet, u16 mode);
void func_801530B0(POLY_GT4 *packet, u16 mode);
u16 func_80156AD4(s16 value);
void func_80156B40(u16 value, u16 *digits);
void func_80156E58(u8 *color, s16 width, SVECTOR *positions, u16 count, s16 bias);
void func_80156FA4(u8 *color, SVECTOR *vertices, u16 flags, u16 mode);

#endif
