#ifndef MEMORIES_DECOMP_DUEL_PACKET_HELPERS_H
#define MEMORIES_DECOMP_DUEL_PACKET_HELPERS_H

#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "drawing_helpers.h"

extern SVECTOR D_8015B7F8;
extern u16 D_8015B800;

void func_80152EC4(POLY_FT4 *packet, u16 flags);
void func_80152F9C(POLY_FT4 *packet, u16 mode);
void func_801530B0(POLY_GT4 *packet, u16 mode);
void func_801531C4(MATRIX *matrix);

#endif
