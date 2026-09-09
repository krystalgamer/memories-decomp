#ifndef MEMORIES_DECOMP_MOVIE_STREAM_RANGES_H
#define MEMORIES_DECOMP_MOVIE_STREAM_RANGES_H

#include "../types.h"

#define MOVIE_STREAM_COUNT 13

/* One entry per streamed movie. func_8005C388 sums sector_count over every
   entry before the one it wants to reach that movie's start sector, then adds
   the entry's own count to the position it hands to the player. end_frame
   overrides the caller's frame limit when it is non-zero. */
typedef struct {
    u16 sector_count;
    u16 end_frame;
} MovieStreamRange;

/* src/game/movie_stream_ranges.c owns the table at 0x8009151C. */
extern MovieStreamRange gMovie_aStreamRanges[MOVIE_STREAM_COUNT];

#endif
