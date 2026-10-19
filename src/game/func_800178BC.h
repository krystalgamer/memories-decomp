#ifndef MEMORIES_DECOMP_FUNC_800178BC_H
#define MEMORIES_DECOMP_FUNC_800178BC_H

#include "../types.h"

/* Initializes a center-relative projection of the fixed ground point
 * (1000, 0, 1000) under the reference view. The two offsets are stored in
 * D_8009B200/D_8009B202; their later purpose is not established here.
 *
 * func_80017130 resets the view defaults before and after sampling; this
 * does not save and restore the caller's prior view. Duel_InitScene then
 * applies the active side's angle separately.
 *
 * Duel_InitScene (src/game/duel_init_scene.c) is the observed consumer and
 * reached it through a local extern that already agreed with this. */
void Duel_InitGroundProjectionReference(void);
extern s16 D_8009B200;
extern s16 D_8009B202;

#endif
