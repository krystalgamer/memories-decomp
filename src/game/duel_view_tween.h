#ifndef MEMORIES_DECOMP_DUEL_VIEW_TWEEN_H
#define MEMORIES_DECOMP_DUEL_VIEW_TWEEN_H

#include "../types.h"

/* Starts a linear tween of the view state at D_800F2848 over `frames` frames.
 * It reads the four current channel values, records them and the targets, and
 * stores a 16.16 fixed-point per-frame delta for each; advancing the tween is
 * someone else's job.
 *
 * Channels are radial distance, the orbit's second angle, heading and the
 * reference target's Z coordinate. Inputs remain full words; targets keep
 * their original halfword storage. Heading interpolation stays linear,
 * with no added wrap or frame validation. */
void Duel_StartViewTween(
    s32 frames, s32 distance, s32 orbit_angle, s32 heading, s32 target_z);

/* Remaining frames in the view tween. The producer treats the count as
 * signed; Duel_InitScene only clears the same 16-bit slot and uses an unsigned
 * view. Both compile to the same gp-relative halfword access. */
#ifdef D_8009B204_UNSIGNED
extern u16 gDuel_wViewTweenFramesRemaining;
#else
extern s16 gDuel_wViewTweenFramesRemaining;
#endif

/* State that Duel_UpdateDisplayTransition (duel_field_display_objects.c) uses to advance
 * the tween.
 *
 * The four target halfwords are distance, orbit angle, heading and target Z. The four
 * accumulators hold their starting values in 16.16 form with the low half
 * biased by 0x8000, and the final four words are the per-frame deltas. */
extern s16 gDuel_wViewTweenDistanceTarget;
extern s16 gDuel_wViewTweenOrbitAngleTarget;
#ifdef VERSION_JAPAN
extern s16 gDuel_wViewTweenHeadingTarget asm("gJapanese_DuelViewTweenHeadingTarget");
#else
extern s16 gDuel_wViewTweenHeadingTarget;
#endif
extern s16 gDuel_wViewTweenReferenceZTarget;

extern s32 gDuel_dwViewTweenDistanceAccumulator;
extern s32 gDuel_dwViewTweenOrbitAngleAccumulator;
extern s32 gDuel_dwViewTweenHeadingAccumulator;
extern s32 gDuel_dwViewTweenReferenceZAccumulator;

extern s32 gDuel_dwViewTweenDistanceStep;
extern s32 gDuel_dwViewTweenOrbitAngleStep;
#ifdef VERSION_JAPAN
extern s32 gDuel_dwViewTweenHeadingStep asm("gJapanese_DuelViewTweenHeadingStep");
#else
extern s32 gDuel_dwViewTweenHeadingStep;
#endif
extern s32 gDuel_dwViewTweenReferenceZStep;

#endif
