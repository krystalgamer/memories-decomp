#ifndef MEMORIES_DECOMP_MOVIE_STREAM_RANGES_H
#define MEMORIES_DECOMP_MOVIE_STREAM_RANGES_H

#include "../ygo_types.h"

#define MOVIE_STREAM_COUNT 13

/* src/game/movie_stream_ranges.c owns the table at 0x8009151C. */
extern MovieStreamRange gMovie_aStreamRanges[MOVIE_STREAM_COUNT];

/* Base LBA added to the cumulative stream sector counts. */
extern s32 D_800E9EB4;

#endif
