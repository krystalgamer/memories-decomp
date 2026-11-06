#ifndef GAME_GRAPHICS_BASE_FRAME_STEP_H
#define GAME_GRAPHICS_BASE_FRAME_STEP_H

#include "../types.h"

/* Per-frame step for motion, fade and timer countdowns. Main_Init seeds 1;
 * Graphics_SyncFrame stores gGraphics_bExtraFrameStep + 1, so it is nominally
 * 1 or 2. Defined once in graphics_frame.c (formerly D_8009B0D8). Kept in its own header so that
 * graphics_frame.h and graphics_frame_controls.h share one declaration without
 * either pulling in the other's globals (which changes gp-relative codegen).
 * Every consumer builds byte-identically with this one plain declaration. */
extern s32 gGraphics_nBaseFrameStep;

#endif
