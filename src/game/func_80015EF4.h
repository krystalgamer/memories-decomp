#ifndef MEMORIES_DECOMP_FUNC_80015EF4_H
#define MEMORIES_DECOMP_FUNC_80015EF4_H

#include "../types.h"
#include "../psyq/libgpu.h"

/* Draws one 3D duel card: `record` is the 0x1C card record whose first word
 * is its display object, `prim` the lit POLY_GT4 card face, `sprite` the
 * POLY_FT4 projected under it, and `color` a scratch word that receives the
 * object's colour for RotColorDpq. One caller: func_800164FC. */
void func_80015EF4(void *record, POLY_GT4 *prim, POLY_FT4 *sprite, s32 *color);

#endif
