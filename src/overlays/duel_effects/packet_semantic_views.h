#ifndef MEMORIES_DECOMP_DUEL_PACKET_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_PACKET_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

/* Semantic packet views retain the original native contracts and owners. */
extern void DuelEffect_SubmitOffsetQuad(void *primitive, u16 flags) __asm__("func_80152EC4");
extern void DuelEffect_SubmitOffsetFt4(POLY_FT4 *packet, u16 mode) __asm__("func_80152F9C");
extern void DuelEffect_SubmitOffsetGt4(POLY_GT4 *packet, u16 mode) __asm__("func_801530B0");
extern void DuelEffect_InitBaseMatrix(MATRIX *matrix) __asm__("func_801531C4");
extern void DuelEffect_ProjectSubmitFt4(POLY_FT4 *packet, SVECTOR *vertices, s16 bias, u16 mode) __asm__("func_80151218");
extern void DuelEffect_ProjectSubmitGt4(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode) __asm__("func_8015131C");

extern void func_80152EC4(void *primitive, u16 flags);
extern void func_80152F9C(POLY_FT4 *packet, u16 mode);
extern void func_801530B0(POLY_GT4 *packet, u16 mode);
extern void func_801531C4(MATRIX *matrix);
extern void func_80151218(POLY_FT4 *packet, SVECTOR *vertices, s16 bias, u16 mode);
extern void func_8015131C(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode);

#endif
