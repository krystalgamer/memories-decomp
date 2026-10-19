#ifndef MEMORIES_DECOMP_DUEL_FIELD_CARD_RENDER_H
#define MEMORIES_DECOMP_DUEL_FIELD_CARD_RENDER_H

#include "../types.h"
#include "../psyq/libgpu.h"
#include "duel_card.h"

/* Draws one 3D field card: `prim` is the lit POLY_GT4 card face, `sprite` the
 * POLY_FT4 projected under it, and `color` a scratch word that receives the
 * object's colour for RotColorDpq. The table index is deliberately read as
 * s8 for the half-turn test, matching retail even for values >= 0x80.
 * One caller: Duel_DrawFieldCards. */
void Duel_RenderFieldCard3D(DuelCardRecord *record, POLY_GT4 *prim, POLY_FT4 *sprite, s32 *color);

#endif
