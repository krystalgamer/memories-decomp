#ifndef GAME_GRAPHICS_FRAME_CONTROLS_H
#define GAME_GRAPHICS_FRAME_CONTROLS_H

#include "../types.h"

#if defined(MODEL_GRAPHICS_STATE_SCENE_BYTES)
#if defined(MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE)
#error scene byte view requires GP-relative addressing
#endif
extern u8 gGraphics_FrameStepControl[4];
#elif defined(MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE)
extern u8 gGraphics_FrameStepControl __attribute__((section(".data")));
#else
extern u8 gGraphics_FrameStepControl;
#endif

#ifdef D_8009B0D8_IN_DATA
extern s32 gGraphics_nBaseFrameStep __attribute__((section(".data")));
#elif defined(D_8009B0D8_IS_VOLATILE)
extern volatile s32 gGraphics_nBaseFrameStep;
#else
extern s32 gGraphics_nBaseFrameStep;
#endif

extern u8 gGraphics_bSkipDisplaySwap;

#endif
