#ifndef MEMORIES_DECOMP_DUEL_TEXTURE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_TEXTURE_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

extern void DuelEffect_PackTexturePageClut(u16 *output, GsIMAGE *image, s32 mode);
extern void DuelEffect_PackFixedPageClut(u16 *output, GsIMAGE *image);
extern void DuelEffect_DrawFixedTextureQuad(u8 *color, u16 size, SVECTOR *offset);
extern void DuelEffect_DrawAtlasColumnQuad(u16 size, u16 index, SVECTOR *offset);

#endif
