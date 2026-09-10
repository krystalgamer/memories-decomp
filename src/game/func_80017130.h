#ifndef MEMORIES_DECOMP_FUNC_80017130_H
#define MEMORIES_DECOMP_FUNC_80017130_H

#include "../types.h"

/* Resets the shared view state at D_800F2848 to its default camera.
 *
 * It writes the fixed distance, angle and projection values, clears the
 * rotation and the three coordinate fields, pushes the projection distance
 * into the GTE through func_800857C0, and finishes with func_8001352C.
 *
 * func_800178BC (src/candidates/func_800178BC.c) is the only consumer and
 * calls this first: its own header
 * describes it as re-establishing the geometry state *after* func_80017130,
 * so the two run as a pair, this one setting the state and that one
 * rebuilding the matrices from it.
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
