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

/* Remaining frames in the view tween. The producer treats the count as
 * signed; func_800179F4 only clears the same 16-bit slot and uses an unsigned
 * view. Both compile to the same gp-relative halfword access. */
#ifdef D_8009B204_UNSIGNED
extern u16 D_8009B204;
#else
extern s16 D_8009B204;
#endif

/* State consumed by unmatched func_800235C0 to advance the tween.
 *
 * The four target halfwords are X, Z, heading and view rotation. The four
 * accumulators hold their starting values in 16.16 form with the low half
 * biased by 0x8000, and the final four words are the per-frame deltas. */
extern s16 D_8009B1EE;
extern s16 D_8009B192;
extern s16 D_8009B190;
extern s16 D_8009B166;

extern s32 D_8009B1C4;
extern s32 D_8009B15C;
extern s32 D_8009B158;
extern s32 D_8009B224;

extern s32 D_8009B1FC;
extern s32 D_8009B198;
extern s32 D_8009B194;
extern s32 D_8009B168;

#endif
