#ifndef MEMORIES_DECOMP_DUEL_PROJECTION_AXES_H
#define MEMORIES_DECOMP_DUEL_PROJECTION_AXES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

void func_80029684(LINE_G3 *packet, GsOT *ot, SVECTOR *points,
                  long *control, s32 origin, s32 span);
void func_800297DC(LINE_G3 *packet, GsOT *ot, SVECTOR *points,
                  long *control, s32 origin, s32 span);

#endif
