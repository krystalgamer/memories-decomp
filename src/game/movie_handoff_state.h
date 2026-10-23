#ifndef GAME_MOVIE_HANDOFF_STATE_H
#define GAME_MOVIE_HANDOFF_STATE_H

#include "../types.h"

extern u8 gMovie_bCopyDisplayOnStop;
extern s16 gLibGs_sActiveBufferIndex __attribute__((section(".data")));
extern s32 gLibCd_nDeferredStreamInterrupt __attribute__((section(".data")));

#endif
