#ifndef MEMORIES_DECOMP_MDEC_SYNC_H
#define MEMORIES_DECOMP_MDEC_SYNC_H

#include "../types.h"

/* Sets D_8009B064, the flag Movie_WaitAndDecodeFrame also raises at the
 * stream's end frame. While it is set, Movie_DecodeAndPresentFrame lowers
 * the CD mix level in D_8009B065 by 0x40 each frame and reports the movie
 * finished once it reaches zero, so a skip fades the audio out over four
 * frames instead of cutting it. */
void Movie_StartFadeOut(void);
s32 func_8005C5D4(void);

#endif
