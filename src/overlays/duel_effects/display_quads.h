#ifndef MEMORIES_DECOMP_DUEL_DISPLAY_QUADS_H
#define MEMORIES_DECOMP_DUEL_DISPLAY_QUADS_H

#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "drawing_helpers.h"
#include "textured_quads.h"

void func_801573A8(u8 *color, u16 size, s16 depth);
void func_80157494(u8 *color, SVECTOR *offset, u16 width, u16 height, u16 tile);
void func_801575CC(u8 *color, DVECTOR *first, DVECTOR *last, u16 width);

#endif
