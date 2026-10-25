#ifndef MEMORIES_DECOMP_MODEL_TEXTURE_UPLOAD_H
#define MEMORIES_DECOMP_MODEL_TEXTURE_UPLOAD_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Packs a texture-page word from a GsIMAGE's page and CLUT rectangles,
 * biasing both by the side. */
#ifdef VERSION_EUROPE
extern u32 ModelTexture_PackPageClut(int side, int mode, GsIMAGE *params) asm("func_8005EE50");
#else
u32 ModelTexture_PackPageClut(int side, int mode, GsIMAGE *params);
#endif

u32 ModelTexture_PackPageClutCopy(s32 slot, s32 mode, GsIMAGE *image);

/* Frozen matching overlay bodies keep their original spelling. */
#define func_80059A50 ModelTexture_PackPageClutCopy

void ModelTexture_UploadTimFile(char *path, int b, int c, int d, int e,
                                int f, int g);

/* Reads a TIM into the caller's GsIMAGE, uploads its pixels and CLUT, and
 * returns the packed texture-page word. */
#ifdef VERSION_EUROPE
extern s32 ModelTexture_LoadTim(GsIMAGE *data, char *path, s32 mode, s32 arg3,
                                s32 x, s32 y, s32 z, s32 w) asm("func_8005CFD0");
#else
s32 ModelTexture_LoadTim(GsIMAGE *data, char *path, s32 mode, s32 arg3, s32 x,
                         s32 y, s32 z, s32 w);
#endif

#endif
