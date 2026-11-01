#ifndef MEMORIES_DECOMP_DUEL_GEOMETRY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_GEOMETRY_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_SetTransformMatrix(MATRIX *world, MATRIX *saved, SVECTOR *position, SVECTOR *rotation, VECTOR *scale, u16 mode);
extern void DuelEffect_SetScaledLocalScreenMatrix(MATRIX *source, VECTOR *scale);
extern void DuelEffect_CopyMatrixComponents(MATRIX *source, MATRIX *destination);
extern void DuelEffect_InitRandomVectorsXYZ(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count);
extern void DuelEffect_InitRandomVectorsNegatedY(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count);
extern void DuelEffect_InitRandomVectorsXY(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 count);
extern void DuelEffect_InitIndexedQuadVertices(SVECTOR *vertices, u16 top_width, u16 bottom_width, u16 top_height, u16 bottom_height, s16 *widths, s16 *heights);
extern void DuelEffect_InitIndexedQuadToYZero(SVECTOR *vertices, u16 top_width, u16 bottom_width, u16 top_height, s16 *widths, s16 *heights);

extern void DuelEffect_InitCircle32Vertices(u16 radius, SVECTOR *vertices);
extern void DuelEffect_InitHalfStepXYRingVertices(u16 width, u16 height, SVECTOR *vertices, u16 count);
extern void DuelEffect_InitHalfStepXZRingVertices(u16 width, u16 depth, s32 level, s32 offset, SVECTOR *vertices, u16 count);
extern void DuelEffect_InitXZRingVertices(u16 width, u16 depth, s16 height, SVECTOR *vertices, u16 count);
extern void DuelEffect_InitRandomVectorsUnscaled(u16 count, SVECTOR *vertices);
extern void DuelEffect_InitRectVertices(SVECTOR *vertices, s32 width, s16 height);
extern void DuelEffect_InitQuadrantQuadVertices(SVECTOR *vertices, u16 size, u16 index);
extern void DuelEffect_SwapVectorRecords(SVECTOR *first, SVECTOR *second);

extern void DuelEffect_InitParticleVectors(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
extern void DuelEffect_InitParticleVectorsNegatedY(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
extern void DuelEffect_InitOffsetQuadVertices(SVECTOR *vertices, SVECTOR *offset);
extern void DuelEffect_InitSquareVertices(SVECTOR *vertices, u16 size);
extern void DuelEffect_InitXZRectVertices(SVECTOR *vertices, u16 width, u16 height, u16 depth);

#endif
