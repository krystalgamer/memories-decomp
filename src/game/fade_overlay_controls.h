#ifndef GAME_FADE_OVERLAY_CONTROLS_H
#define GAME_FADE_OVERLAY_CONTROLS_H

#include "../types.h"

/* The fade depth byte kept for the overlay while FADE_FLAG_KEEP_OVERLAY is
 * set. */
extern u8 gFade_bKeepOverlayDepth;

/* Latches the white-fade path until the transition completes. Most fade
 * units reach it through small data; Script_OpShowImage defines the arm below
 * to preserve its measured absolute-addressing form. */
extern u8 gFade_bWhiteLatch;

#endif
