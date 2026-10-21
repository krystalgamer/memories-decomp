#ifndef MEMORIES_DECOMP_MDEC_SYNC_H
#define MEMORIES_DECOMP_MDEC_SYNC_H

#include "../types.h"

/* Sets gMovie_bFadingOut, the flag Movie_WaitAndDecodeFrame also raises at the
 * stream's end frame. While it is set, Movie_DecodeAndPresentFrame lowers
 * the CD mix level in gMovie_bCdVolume by 0x40 each frame and reports the movie
 * finished once it reaches zero, so a skip fades the audio out over four
 * frames instead of cutting it. */
void Movie_StartFadeOut(void);
/* Spins until gMovie_bFrameDecoded, the latch Movie_OnDecodeOutput (the DecDCTout callback)
 * sets once the last strip of the frame is uploaded, is raised. After a
 * 0x800000 countdown it gives up and calls DecDCTReset(1). Either way it
 * clears the latch and returns 0. */
s32 Movie_WaitFrameDecoded(void);

#endif
