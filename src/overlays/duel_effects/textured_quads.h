#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TEXTURED_QUADS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TEXTURED_QUADS_H

#include "../../types.h"
#include "utility_helpers.h"

typedef struct {
    u16 page_at_00;
    u16 clut_at_02;
    u16 page_at_04;
    u16 clut_at_06;
    u16 page_at_08;
    u16 clut_at_0A;
    u16 page_at_0C;
    u16 clut_at_0E;
    u8 unknown_10[0xC];
    u16 page_at_1C;
    u16 clut_at_1E;
    u16 page_at_20;
    u16 clut_at_22;
    u8 unknown_24[4];
    u16 page0;
    u16 clut0;
    u16 page1;
    u16 clut1;
} DuelEffectQuadTextureWords;

typedef union {
    DuelEffectQuadTextureWords named;
    u16 pairs[21][2];
} DuelEffectTextureTable;

typedef struct {
    POLY_FT4 polygon;
    SVECTOR vertices[4];
} DuelEffectTexturedQuad;

extern DuelEffectTextureTable D_8015B748;

void func_80151218(POLY_FT4 *polygon, SVECTOR *vertices, s16 depth, u16 flags);
void func_80156C40(u8 *color, u16 size, SVECTOR *offset);
void func_80156D50(u16 size, u16 index, SVECTOR *offset);

#endif
