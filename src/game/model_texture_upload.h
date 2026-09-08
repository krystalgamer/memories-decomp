#ifndef MEMORIES_DECOMP_MODEL_TEXTURE_UPLOAD_H
#define MEMORIES_DECOMP_MODEL_TEXTURE_UPLOAD_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Packs a texture-page word from a GsIMAGE's page and CLUT rectangles,
 * biasing both by the side. */
u32 func_80058A7C(int side, int mode, GsIMAGE *params);

/* Reads a TIM into the caller's GsIMAGE, uploads its pixels and CLUT, and
 * returns the packed texture-page word. */
s32 func_80058B4C(GsIMAGE *data, s32 arg1, s32 mode, s32 arg3, s32 x, s32 y,
                  s32 z, s32 w);

#endif
