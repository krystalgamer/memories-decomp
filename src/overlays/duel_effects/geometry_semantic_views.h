#ifndef MEMORIES_DECOMP_DUEL_GEOMETRY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_GEOMETRY_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

/* Semantic views leave the fingerprinted native helper header unchanged. */
extern void DuelEffect_SetTransformMatrix(MATRIX *world, MATRIX *saved, SVECTOR *position, SVECTOR *rotation, VECTOR *scale, u16 mode) __asm__("func_801513F4");
extern void DuelEffect_SetScaledLocalScreenMatrix(MATRIX *source, VECTOR *scale) __asm__("func_801514BC");
extern void DuelEffect_CopyMatrixComponents(MATRIX *source, MATRIX *destination) __asm__("func_801514F8");
extern void DuelEffect_InitRandomVectorsXYZ(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count) __asm__("func_8014F608");
extern void DuelEffect_InitRandomVectorsNegatedY(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count) __asm__("func_8014F754");
extern void DuelEffect_InitRandomVectorsXY(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 count) __asm__("func_8014F89C");
extern void DuelEffect_InitIndexedQuadVertices(SVECTOR *vertices, u16 top_width, u16 bottom_width, u16 top_height, u16 bottom_height, s16 *widths, s16 *heights) __asm__("func_8014F9A0");
extern void DuelEffect_InitIndexedQuadToYZero(SVECTOR *vertices, u16 top_width, u16 bottom_width, u16 top_height, s16 *widths, s16 *heights) __asm__("func_8014FA3C");

extern void func_801513F4(MATRIX *world, MATRIX *saved, SVECTOR *position, SVECTOR *rotation, VECTOR *scale, u16 mode);
extern void func_801514BC(MATRIX *source, VECTOR *scale);
extern void func_801514F8(MATRIX *source, MATRIX *destination);
extern void func_8014F608(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count);
extern void func_8014F754(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count);
extern void func_8014F89C(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 count);
extern void func_8014F9A0(SVECTOR *vertices, u16 top_width, u16 bottom_width, u16 top_height, u16 bottom_height, s16 *widths, s16 *heights);
extern void func_8014FA3C(SVECTOR *vertices, u16 top_width, u16 bottom_width, u16 top_height, s16 *widths, s16 *heights);

#endif
