#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_TIMING_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_TIMING_H

#include "../types.h"
#include "sound.h"

/* Opens one sequence track from its MTrk header, as sound.h's SDSequenceTrack
 * notes describe, and returns whether it opened.
 *
 * SD_ScaleSequenceDelta rescales the pending delta for the sequence timebase,
 * carrying the track's remainder in its mask/shift cases, but not its /3 cases.
 * SD_ProcessSequenceTracks, later in sound_sequence_tracks.c, runs the same step
 * over its own track walk. */
int SD_OpenSequenceTrack(SDSequenceTrack *entry);
void SD_ScaleSequenceDelta(SDSequenceTrack *entry);

/* Starts the staged secondary sequence, using the low byte of start to choose
 * its playback state: nonzero plays (sequence_state = 1), zero leaves it paused
 * (sequence_state = 4, the state SD_MuteSecondaryVoices sets). The second argument is
 * never read; the command pump's retail call still loads $a1 with 1. */
void SD_PlaySequence(s32 start, s32 unused);

s32 SD_StartSequenceTracks(void);

#endif
