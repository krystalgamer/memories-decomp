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

extern void DuelEffect_InitCircle32Vertices(u16 radius, SVECTOR *vertices) __asm__("func_8014EA7C");
extern void DuelEffect_InitHalfStepXYRingVertices(u16 width, u16 height, SVECTOR *vertices, u16 count) __asm__("func_8014EB1C");
extern void DuelEffect_InitHalfStepXZRingVertices(u16 width, u16 depth, s32 level, s32 offset, SVECTOR *vertices, u16 count) __asm__("func_8014EC8C");
extern void DuelEffect_InitXZRingVertices(u16 width, u16 depth, s16 height, SVECTOR *vertices, u16 count) __asm__("func_8014EE0C");
extern void DuelEffect_InitRandomVectorsUnscaled(u16 count, SVECTOR *vertices) __asm__("func_8014EF2C");
extern void DuelEffect_InitRectVertices(SVECTOR *vertices, s32 width, s16 height) __asm__("func_8014F490");
extern void DuelEffect_InitQuadrantQuadVertices(SVECTOR *vertices, u16 size, u16 index) __asm__("func_8014FE00");
extern void DuelEffect_SwapVectorRecords(SVECTOR *first, SVECTOR *second) __asm__("func_8014FED4");

extern void func_8014EA7C(u16 radius, SVECTOR *vertices);
extern void func_8014EB1C(u16 width, u16 height, SVECTOR *vertices, u16 count);
extern void func_8014EC8C(u16 width, u16 depth, s32 level, s32 offset, SVECTOR *vertices, u16 count);
extern void func_8014EE0C(u16 width, u16 depth, s16 height, SVECTOR *vertices, u16 count);
extern void func_8014EF2C(u16 count, SVECTOR *vertices);
extern void func_8014F490(SVECTOR *vertices, s32 width, s16 height);
extern void func_8014FE00(SVECTOR *vertices, u16 size, u16 index);
extern void func_8014FED4(SVECTOR *first, SVECTOR *second);

extern void DuelEffect_InitParticleVectors(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count) __asm__("func_8014F030");
extern void DuelEffect_InitParticleVectorsNegatedY(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count) __asm__("func_8014F180");
extern void DuelEffect_InitOffsetQuadVertices(SVECTOR *vertices, SVECTOR *offset) __asm__("func_8014F2D4");
extern void DuelEffect_InitSquareVertices(SVECTOR *vertices, u16 size) __asm__("func_8014F358");
extern void DuelEffect_InitXZRectVertices(SVECTOR *vertices, u16 width, u16 height, u16 depth) __asm__("func_8014F3E8");

extern void func_8014F030(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
extern void func_8014F180(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
extern void func_8014F2D4(SVECTOR *vertices, SVECTOR *offset);
extern void func_8014F358(SVECTOR *vertices, u16 size);
extern void func_8014F3E8(SVECTOR *vertices, u16 width, u16 height, u16 depth);

#endif
