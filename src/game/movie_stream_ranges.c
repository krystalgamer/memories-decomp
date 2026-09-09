#include "../types.h"
#include "movie_stream_ranges.h"

/* Initialized data at 0x8009151C: only the first movie carries a range; the
   rest are reached through the running sum, which is zero for all of them. */
MovieStreamRange gMovie_aStreamRanges[MOVIE_STREAM_COUNT] = {
    { 0x0C08, 0x012F },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
};
