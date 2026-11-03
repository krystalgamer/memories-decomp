#ifndef GAME_MOVIE_HANDOFF_STATE_H
#define GAME_MOVIE_HANDOFF_STATE_H

#include "../types.h"

extern u8 gMovie_bCopyDisplayOnStop;
extern s16 gLibGs_sActiveBufferIndex __attribute__((section(".data")));
/* The pending-interrupt word Movie_OnDecodeOutput tests, and clears after calling
 * StCdInterrupt, while the stream is running (gMovie_bRgb24). Four bytes, so at
 * -G8 a plain declaration would land in sdata and both accesses would come out
 * gp-relative; retail reaches it through lui %hi / %lo, which the .data
 * section attribute keeps. Movie_OnDecodeOutput is its only C consumer. */
extern s32 gLibCd_nDeferredStreamInterrupt __attribute__((section(".data")));

#endif
