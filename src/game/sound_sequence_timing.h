#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_TIMING_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_TIMING_H

#include "../types.h"
#include "sound.h"

/* The definition returns s32 and takes no arguments. The secondary playback
 * unit has two distinct retail call sequences to the same symbol: one sets
 * $a0 to the state pointer and one leaves $a0 untouched. Both caller views
 * remain void-returning so ignored-result allocation stays unchanged. */
#ifdef SD_START_SEQUENCE_TRACKS_PLAYBACK_CALLS
void SD_StartSequenceTracks(SDSecondaryState *state);
void SD_StartSequenceTracks_no_arg(void) asm("SD_StartSequenceTracks");
#else
s32 SD_StartSequenceTracks(void);
#endif

#endif
