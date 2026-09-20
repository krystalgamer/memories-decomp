#ifndef MEMORIES_DECOMP_FUNC_8005C62C_H
#define MEMORIES_DECOMP_FUNC_8005C62C_H

#include "../types.h"
#include "../psyq/libcd.h"

/* Re-issues the disc read at `loc`: sets the location, seeks to it, and
 * starts a 0x1E0-sector streaming read. Both callers hand it the movie
 * stream position D_8009B49C, which movie_frame_pipeline.h declares CdlLOC.
 *
 * The two libcd calls take the location as bytes and the libds read takes
 * DslLOC, which is the same four fields under the other library's name, so
 * those three casts stay -- but the parameter no longer has to be an
 * integer for them. */
void Movie_SeekAndStartRead(CdlLOC *loc);

#endif
