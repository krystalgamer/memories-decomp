#ifndef MEMORIES_DECOMP_DUEL_DRAWING_TAIL_H
#define MEMORIES_DECOMP_DUEL_DRAWING_TAIL_H

#include "../../types.h"
#include "../../psyq/rand.h"
#include "packet_helpers.h"
#include "textured_quads.h"

void func_80155F94(u8 *color);
void func_80156064(u8 *color, SVECTOR *vertices, s16 depth);
void func_8015616C(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, s16 mode);

#endif
