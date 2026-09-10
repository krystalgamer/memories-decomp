#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_PARSER_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_PARSER_H

#include "../types.h"
#include "sound.h"

s32 SD_ReadSequenceU32BE(SDSequenceTrack *input);
s32 SD_ReadSequenceU16BE(SDSequenceTrack *input);

/* Track and tempo stream setup on D_8009B458. It resets the record's stream
 * fields, reads the timebase and the raw tempo out of the sequence, derives
 * the tempo step from them, scales it by the timebase mode, and stores it
 * before handing the track to SD_ReadSequenceByte.
 *
 * The return type is s32 and the function returns 1 unconditionally. That is
 * the definition's own spelling, and it is what this header carries. The
 * local declaration it replaces said `void`, which nothing could catch: the
 * sole caller, SD_StartSequenceTracks, discards the result, so no call site
 * ever compared the two. Declaring it here, in a header the defining unit
 * includes, makes the compiler check it on every build.
 *
 * What this does NOT claim: that the returned 1 means anything. No caller
 * reads it, so the value is unexamined rather than understood. */
s32 SD_ReadSequenceHeader(void);

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
