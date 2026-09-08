#ifndef MEMORIES_DECOMP_GPU_PACKETS_H
#define MEMORIES_DECOMP_GPU_PACKETS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Copies a primitive, adds draw mode, and links it into the ordering table. */
void func_8005B260(u32 *src, GsOT *ot, s32 idx, s32 flags);

#endif
