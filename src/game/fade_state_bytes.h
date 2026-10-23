#ifndef GAME_FADE_STATE_BYTES_H
#define GAME_FADE_STATE_BYTES_H

#include "../types.h"

#ifdef D_800E9ECE_AS_SCALAR
extern u8 gFade_bFlags;
#else
extern u8 gFade_bFlags[];
#endif

#ifdef D_800E9ECF_AS_SCALAR
extern u8 gFade_bStep;
#else
extern u8 gFade_bStep[];
#endif

#endif
