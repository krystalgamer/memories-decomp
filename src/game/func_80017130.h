#ifndef MEMORIES_DECOMP_FUNC_80017130_H
#define MEMORIES_DECOMP_FUNC_80017130_H

#include "../types.h"

/* Resets the shared view state at D_800F2848 to its default camera.
 *
 * It writes the fixed distance, angle and projection values, clears the
 * rotation and the three coordinate fields, pushes the projection distance
 * into the GTE through GsSetProjection, and finishes with ViewState_ApplyOrbit.
 *
 * Duel_InitGroundProjectionReference (src/game/func_800178BC.c) is the
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
 * The name stays address-based. What the function resets is legible, but
 * which camera or screen owns the default is not settled here. */
void func_80017130(void);

#endif
