#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_TIMING_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_TIMING_H

#include "../types.h"
#include "sound.h"

/* Opens one sequence track from its MTrk header, as sound.h's SDSequenceTrack
 * notes describe, and returns whether it opened.
 *
 * SD_ScaleSequenceDelta rescales that track's pending delta after a tempo or
 * resolution change. sd_sequence_tracks.c runs the same step over its own
 * track walk and used to declare the function itself, in the identical
 * spelling. */
int SD_OpenSequenceTrack(SDSequenceTrack *entry);
void SD_ScaleSequenceDelta(SDSequenceTrack *entry);

/* Starts the staged secondary sequence, using the low byte of start to choose
 * its playback state. The definition consumes only start. The command pump
 * also sets $a1 to 1 in retail, so it selects the same ambient-argument arm
 * as the other secondary steps in sound.h, before including either header. */
#ifdef SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
void func_80049AF4(s32 start, s32 ambient);
#else
void func_80049AF4(s32 start);
#endif

/* The definition returns s32 and takes no arguments. Secondary playback has
 * two retail register setups: one leaves the state pointer in $a0 and the
 * other leaves $a0 untouched. func_80049AF4 uses the canonical no-argument
 * declaration; its preceding state writes already reproduce the first setup.
 * The older void-returning views remain for callers that still select them. */
#ifdef SD_START_SEQUENCE_TRACKS_PLAYBACK_CALLS
void SD_StartSequenceTracks(SDSecondaryState *state);
#else
s32 SD_StartSequenceTracks(void);
#endif
extern void SD_StartSequenceTracks_no_arg(void) asm("SD_StartSequenceTracks");

#endif
