#ifndef MEMORIES_DECOMP_GPU_PACKETS_H
#define MEMORIES_DECOMP_GPU_PACKETS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Packet builders advance the cursor as u32 words. The model-capacity query
 * needs the same storage loaded as a forced-.data integer address to preserve
 * its retail address construction. */
#ifdef GPU_PACKET_CURSOR_AS_ADDRESS
extern s32 D_800FE240 __attribute__((section(".data")));
#else
extern u32 *D_800FE240 __attribute__((section(".data")));
#endif

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
