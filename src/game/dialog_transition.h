#ifndef MEMORIES_DECOMP_DIALOG_TRANSITION_H
#define MEMORIES_DECOMP_DIALOG_TRANSITION_H

#include "../types.h"
#include "menu_record.h"

/* Dialog transition callbacks, also present in gDisplayEffect_apfnSteps.
 * Their first-entry latch and progress bits share D_8009B3C1 with the
 * memory-card controller; this is not per-record state.
 *
 * Contraction and open create a spotlight mask and decrease its radii.
 * Close moves/releases the primary sprite and expands/releases that mask.
 * Objects use the existing integer row slots; the channel index stays a
 * byte view at +0x1A. Japan retains the 0x60-byte channel stride. */
void Dialog_UpdateSpotlightContraction(MenuRecord *state);
void Dialog_UpdateCloseTransition(MenuRecord *state);
void Dialog_UpdateOpenTransition(MenuRecord *state);

#endif
