#ifndef MEMORIES_DECOMP_GPU_PACKETS_H
#define MEMORIES_DECOMP_GPU_PACKETS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Copies a primitive, adds draw mode, and links it into the ordering table. */
void func_8005B260(u32 *src, GsOT *ot, s32 idx, s32 flags);

/* The texture-window variant: brackets the primitive with a 0xE2 window word
   built from the offset and mask pairs. */
void func_8005B36C(u32 *src, GsOT *ot, s32 idx, s32 offx, s32 offy,
                   s32 maskx, s32 masky);

/* The mask-write variant: brackets the primitive with a 0xE6 pair that turns
   mask-bit drawing on before it and off after. */
void func_8005B4D8(u32 *src, GsOT *ot, s32 idx, s32 flags);

#endif
