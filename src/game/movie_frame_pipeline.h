#ifndef MEMORIES_DECOMP_MOVIE_FRAME_PIPELINE_H
#define MEMORIES_DECOMP_MOVIE_FRAME_PIPELINE_H

#include "../types.h"
#include "../psyq/libcd.h"

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

/* The pipeline's state block, 0x8009B060 to 0x8009B073 in one run, plus the
 * two stream fields after it. Every symbol below was declared identically by
 * each of its users and none is named outside this family.
 *
 * The eight bytes are flags and small counters: D_8009B060 is set from
 * func_8005B8A0's argument and gated on, D_8009B064 is cleared on start and
 * read by the fade path, and D_8009B062 is the one mdec_sync.c shares.
 * D_8009B068, D_8009B06C and D_8009B070 are the three words that follow.
 *
 * D_8009B498 is the VLC buffer -- func_8005B8A0 points it at D_80010000 and
 * DecDCTvlcBuild is handed it -- and D_8009B49C is the stream position, copied
 * from a CdlLOC and passed on by address. This header now includes
 * psyq/libcd.h for that type rather than relying on its consumers having done
 * so; mdec_sync.c had not.
 */
extern u8 D_8009B060;
extern u8 D_8009B061;
extern u8 D_8009B062;
extern u8 D_8009B063;
extern u8 D_8009B064;
extern u8 D_8009B065;
extern u8 D_8009B066;
extern u8 D_8009B067;
extern u32 D_8009B068;
extern u32 D_8009B06C;
extern u32 D_8009B070;
extern u8 *D_8009B498;
extern CdlLOC D_8009B49C;

#endif
