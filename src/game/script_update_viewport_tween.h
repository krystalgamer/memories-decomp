#ifndef MEMORIES_DECOMP_SCRIPT_UPDATE_VIEWPORT_TWEEN_H
#define MEMORIES_DECOMP_SCRIPT_UPDATE_VIEWPORT_TWEEN_H

#include "../types.h"

/* D_80090C50 handler: the two-axis smooth scroll stepper Script_OpViewportTween
 * hands the scene over to. On its first frame it derives the per-frame 16.16
 * deltas from the distance to the target over the remaining frame count, then
 * advances both accumulators, publishes their high halves as the camera
 * position, and snaps to the target when the counter runs out. */
void Script_UpdateViewportTween(void);

#endif
