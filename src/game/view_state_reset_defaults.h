#ifndef MEMORIES_DECOMP_VIEW_STATE_RESET_DEFAULTS_H
#define MEMORIES_DECOMP_VIEW_STATE_RESET_DEFAULTS_H

#include "../types.h"

/* Applies the measured defaults to shared ViewState, projection and view.
 *
 * It writes the fixed distance, angle and projection values, clears the
 * rotation and the three coordinate fields, pushes the projection distance
 * into the GTE through GsSetProjection, and finishes with ViewState_ApplyOrbit.
 *
 * Duel_InitGroundProjectionReference (src/game/duel_ground_projection.c) is the
 * observed consumer and calls this before and after its reference sample.
 * The second call returns the view to these defaults rather than restoring
 * an arbitrary prior camera state.
 *
 * The definition holds a second base pointer into the same structure, used
 * for three of the stores. That is load bearing rather than leftover: the
 * unit's own comment records that spelling those three through the first base
 * comes out one instruction short of retail. It should not be folded away
 * without a measurement.
 *
 * The neutral shared-state name does not assign exclusive screen ownership
 * or claim that every byte of a larger backing allocation is cleared. */
void ViewState_ResetDefaults(void);

#endif
