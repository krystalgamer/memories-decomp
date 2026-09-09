#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_EVENTS_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_EVENTS_H

#include "../types.h"
#include "sound.h"

/* Advances a track's read cursor. */
void SD_AdvanceSequencePosition(s32 *value, s32 amount);

/* The default arm of SD_ReadSequenceEvent: an event the driver does not
   handle is consumed and nothing happens. The body is empty. */
void SD_IgnoreSequenceEvent(void);

/* Meta events. `arg1` is the meta type byte the reader has already fetched;
   end-of-track and tempo are handled, the rest are skipped by length. */
void SD_HandleSequenceMetaEvent(SDSequenceTrack *p, s32 arg1);

/* Reads the SysEx length and consumes bytes until the terminator. */
void SD_SkipSequenceSysEx(SDSequenceTrack *input);

/* Channel voice events. `status` is the status byte, `d1` and `d2` the one or
   two data bytes the reader fetched; both are u8, which is narrower than the
   s32 the caller had been declaring. */
void SD_DispatchSequenceChannelEvent(SDSequenceTrack *p, s32 status, u8 d1, u8 d2);

#endif
