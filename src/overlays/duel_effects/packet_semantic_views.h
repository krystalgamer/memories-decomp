#ifndef MEMORIES_DECOMP_DUEL_PACKET_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_PACKET_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

extern void DuelEffect_SubmitOffsetQuad(void *primitive, u16 flags);
extern void DuelEffect_SubmitOffsetFt4(POLY_FT4 *packet, u16 mode);
extern void DuelEffect_SubmitOffsetGt4(POLY_GT4 *packet, u16 mode);
extern void DuelEffect_InitBaseMatrix(MATRIX *matrix);
extern void DuelEffect_ProjectSubmitFt4(POLY_FT4 *packet, SVECTOR *vertices, s16 bias, u16 mode);
extern void DuelEffect_ProjectSubmitGt4(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode);

#endif
