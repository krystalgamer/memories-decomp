#ifndef MEMORIES_DECOMP_MODEL_TEXTURE_UPLOAD_H
#define MEMORIES_DECOMP_MODEL_TEXTURE_UPLOAD_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Packs a page/CLUT word from a GsIMAGE, biasing its rectangles by side.
 * blend_mode supplies the texture-page ABR bits. */
#ifdef VERSION_EUROPE
extern u32 ModelTexture_PackPageClut(int side, int blend_mode, GsIMAGE *params) asm("func_8005EE50");
#else
u32 ModelTexture_PackPageClut(int side, int blend_mode, GsIMAGE *params);
#endif

u32 ModelTexture_PackPageClutCopy(s32 side, s32 blend_mode, GsIMAGE *image);

/* Frozen matching overlay bodies keep their original spelling. */
#define func_80059A50 ModelTexture_PackPageClutCopy

void ModelTexture_UploadTimFile(char *path, int destination_mode, int blend_mode, int pixel_x, int pixel_y,
                                int clut_x, int clut_y);

/* Reads a TIM into the caller's GsIMAGE and uploads pixels then CLUT.
 * destination_mode selects placement; blend_mode supplies page ABR bits.
 * Returns the packed page/CLUT word. */
#ifdef VERSION_EUROPE
extern s32 ModelTexture_LoadTim(GsIMAGE *data, char *path, s32 destination_mode, s32 blend_mode,
                                s32 pixel_x, s32 pixel_y, s32 clut_x, s32 clut_y) asm("func_8005CFD0");
#else
s32 ModelTexture_LoadTim(GsIMAGE *data, char *path, s32 destination_mode, s32 blend_mode, s32 pixel_x,
                         s32 pixel_y, s32 clut_x, s32 clut_y);
#endif

#endif
