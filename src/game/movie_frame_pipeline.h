#ifndef MEMORIES_DECOMP_MOVIE_FRAME_PIPELINE_H
#define MEMORIES_DECOMP_MOVIE_FRAME_PIPELINE_H

#include "../types.h"

/* The movie frame pipeline's entry points.
 *
 * func_8005BB7C and func_8005BFC8 both return a status their callers are
 * free to ignore, and both did: one caller declared func_8005BB7C as
 * returning void, and func_8005BFC8 was called with no declaration at all.
 *
 * func_8005C1F4 is installed as a Psy-Q decoder timeout callback through
 * DecDCToutCallback, so its exact void(void) shape is load bearing. */
s32 func_8005BB7C(s32 arg0);
s32 func_8005BE3C(void);
s32 func_8005BFC8(s32 resync);
void func_8005C1F4(void);
void func_8005C374(s32 first, s32 second, s32 third);

#endif
