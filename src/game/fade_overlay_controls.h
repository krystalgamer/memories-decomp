#ifndef GAME_FADE_OVERLAY_CONTROLS_H
#define GAME_FADE_OVERLAY_CONTROLS_H

#include "../types.h"

#ifdef D_8009B140_IN_DATA
extern u8 gFade_bKeepOverlayDepth __attribute__((section(".data")));
#else
extern u8 gFade_bKeepOverlayDepth;
#endif

#ifdef D_8009B145_IN_DATA
extern u8 gFade_bWhiteLatch __attribute__((section(".data")));
#else
extern u8 gFade_bWhiteLatch;
#endif

#endif
