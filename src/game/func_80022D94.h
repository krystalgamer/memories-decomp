#ifndef MEMORIES_DECOMP_FUNC_80022D94_H
#define MEMORIES_DECOMP_FUNC_80022D94_H

#include "../types.h"

/* Starts a linear tween of the view state at D_800F2848 over `frames` frames.
 * It reads the four current channel values, records them and the targets, and
 * stores a 16.16 fixed-point per-frame delta for each; advancing the tween is
 * someone else's job.
 *
 * The parameter names are the definition's own. `y` is worth a caveat: it
 * targets D_800F2848.angle, which view_state.h establishes is the camera
 * heading in the game's 0x1000-unit turn space rather than a coordinate. The
 * spelling is the original author's rather than evidence. */
void func_80022D94(s32 frames, s32 x, s32 z, s32 y, s32 value);

#endif
