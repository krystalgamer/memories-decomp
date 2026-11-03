#ifndef GAME_FADE_OVERLAY_CONTROLS_H
#define GAME_FADE_OVERLAY_CONTROLS_H

#include "../types.h"

/* The fade depth byte kept for the overlay while FADE_FLAG_KEEP_OVERLAY is
 * set. Most of the fade family reach it through $gp (small data) and take
 * the plain arm below. build_deck_update.c and src/game/duel_effect_states.c
 * address it with %hi/%lo instead, outside small data, so they take the
 * .data arm. Both set it from gDisplayObject_awOrderingTableDepthBases[1]
 * (display_object_helpers.h) and take that table's .data arm for the same
 * reason. */
#ifdef D_8009B140_IN_DATA
extern u8 gFade_bKeepOverlayDepth __attribute__((section(".data")));
#else
extern u8 gFade_bKeepOverlayDepth;
#endif

/* Latches the white-fade path until the transition completes. Most fade
 * units reach it through small data; Script_OpShowImage defines the arm below
 * to preserve its measured absolute-addressing form. */
#ifdef D_8009B145_IN_DATA
extern u8 gFade_bWhiteLatch __attribute__((section(".data")));
#else
extern u8 gFade_bWhiteLatch;
#endif

#endif
