#ifndef MEMORIES_DECOMP_DUEL_TEXTURE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_TEXTURE_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

/* Typed texture views preserve original native owners and packet contracts. */
extern void DuelEffect_PackTexturePageClut(u16 *output, GsIMAGE *image, s32 mode) __asm__("func_8014F564");
extern void DuelEffect_PackFixedPageClut(u16 *output, GsIMAGE *image) __asm__("func_8014F5D0");
extern void DuelEffect_DrawFixedTextureQuad(u8 *color, u16 size, SVECTOR *offset) __asm__("func_80156C40");
extern void DuelEffect_DrawAtlasColumnQuad(u16 size, u16 index, SVECTOR *offset) __asm__("func_80156D50");

extern void func_8014F564(u16 *output, GsIMAGE *image, s32 mode);
extern void func_8014F5D0(u16 *output, GsIMAGE *image);
extern void func_80156C40(u8 *color, u16 size, SVECTOR *offset);
extern void func_80156D50(u16 size, u16 index, SVECTOR *offset);

#endif
