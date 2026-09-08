#ifndef YUGIOH_GAME_GRAPHICS_FRAME_H
#define YUGIOH_GAME_GRAPHICS_FRAME_H

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

extern DISPENV gGraphics_DispEnv;

/* Viewport scroll offset in pixels, signed: func_8002A660 subtracts it from a
   sprite position and func_80040588 subtracts it from a primitive's, and both
   results have to be able to go negative.

   Files that reach these through `__attribute__((section(".data")))`, or as an
   unsized or [4] array, are deliberately not converted -- those spellings
   change how the address is materialised, not just how the value reads. */
extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;

void Graphics_SyncFrame(void);
void Graphics_BeginFrame(void);

#endif
