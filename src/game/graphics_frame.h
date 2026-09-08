#ifndef YUGIOH_GAME_GRAPHICS_FRAME_H
#define YUGIOH_GAME_GRAPHICS_FRAME_H

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

extern DISPENV gGraphics_DispEnv;

void Graphics_SyncFrame(void);
void Graphics_BeginFrame(void);

#endif
