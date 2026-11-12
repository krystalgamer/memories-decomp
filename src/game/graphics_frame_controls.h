#ifndef GAME_GRAPHICS_FRAME_CONTROLS_H
#define GAME_GRAPHICS_FRAME_CONTROLS_H

#include "../types.h"

extern u8 gGraphics_FrameStepControl;

/* The three bytes after the frame-step override are separate u8 globals, not
 * one aggregate: GCC 2.8 word-aligns struct and array definitions, yet the
 * Japanese build has the four bytes at the unaligned 0x8009AF0B. */
extern u8 D_8009AFA6;
extern u8 gModel_bCreditsActiveSlot;

#include "main.h"

extern u8 gGraphics_bSkipDisplaySwap;

#endif
