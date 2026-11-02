#ifndef MEMORIES_DECOMP_DUEL_INIT_GROUND_PROJECTION_REFERENCE_H
#define MEMORIES_DECOMP_DUEL_INIT_GROUND_PROJECTION_REFERENCE_H

#include "../types.h"

/* Initializes a center-relative projection of the fixed ground point
 * (1000, 0, 1000) under the reference view. The projected screen x and y,
 * less 0xA0 and 0x6C, are stored in
 * gDuel_sGroundReferenceScreenX and gDuel_sGroundReferenceScreenY; no
 * matched source reads them back.
 *
 * ViewState_ResetDefaults runs before and after sampling; this
 * does not save and restore the caller's prior view. Duel_InitScene then
 * applies the active side's angle separately.
 *
 * Duel_InitScene (src/game/duel_init_scene.c) is the observed consumer and
 * reached it through a local extern that already agreed with this. */
void Duel_InitGroundProjectionReference(void);
extern s16 gDuel_sGroundReferenceScreenX;
extern s16 gDuel_sGroundReferenceScreenY;

#endif
