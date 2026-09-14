#ifndef MEMORIES_DECOMP_MOVIE_PLAYBACK_CONTROL_H
#define MEMORIES_DECOMP_MOVIE_PLAYBACK_CONTROL_H

#include "../types.h"

/* Starting, polling and clearing movie playback.
 *
 * The three share one state byte, D_8009B318: Movie_Play sets its 0x80 bit
 * when a movie starts, func_800359B0 sets 0x40 once the stream has been asked
 * to stop and clears 0x80 when it has, and Movie_ResetPlaybackState clears
 * the byte outright at boot. They are three steps over one flag rather than
 * three unrelated entry points, which is what a caller seeing only its own
 * prototype cannot tell. */
void Movie_Play(s32 index);
void func_800359B0(void);
void Movie_ResetPlaybackState(void);

#endif
