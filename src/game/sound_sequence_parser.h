#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_PARSER_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_PARSER_H

#include "../types.h"
#include "sound.h"

/* Byte comparison, the "MTrk" chunk search and the byte and variable-length
 * readers every sequence stream goes through. */
s32 SD_CompareBytes(u8 *left, u8 *right, s32 count);
s32 SD_FindMidiTrackChunk(s32 offset);
s32 SD_ReadSequenceByte(SDSequenceTrack *reader);
s32 SD_ReadVariableLengthValue(SDSequenceTrack *input);

s32 SD_ReadSequenceU32BE(SDSequenceTrack *input);
s32 SD_ReadSequenceU16BE(SDSequenceTrack *input);

/* Track and tempo stream setup on gSD_pSecondaryState. It resets the record's stream
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
   handle is consumed and nothing happens. The body is empty, but the caller
   passes the unhandled status byte: retail zero-extends it into $a0 before
   the call, and that argument decides the caller's register allocation. */
void SD_IgnoreSequenceEvent(u8 status);

/* Meta events. `meta_type` is the byte the reader has already fetched;
   end-of-track and tempo are handled, the rest are skipped by length. */
void SD_HandleSequenceMetaEvent(SDSequenceTrack *p, s32 meta_type);

/* Reads the SysEx length and consumes bytes until the terminator. */
void SD_SkipSequenceSysEx(SDSequenceTrack *input);

/* Channel voice events. `status` is the status byte, `d1` and `d2` the one or
   two data bytes the reader fetched; both are u8, which is narrower than the
   s32 the caller had been declaring. */
void SD_DispatchSequenceChannelEvent(SDSequenceTrack *p, s32 status, u8 d1, u8 d2);

/* Reads one event from a sequence track and returns its decoded form.
 *
 * It takes the next byte through SD_ReadSequenceByte and treats it as a
 * status byte only when SD_SEQUENCE_STATUS_BIT is set. When the bit is clear
 * the byte is data and the track's stored running_status is reused, with
 * running_status_held set to say so; when it is set the byte becomes the new
 * running status, except for a meta event, which is used for this event
 * without replacing the stored one. That is the running-status convention a
 * MIDI-style stream uses to omit repeated status bytes.
 *
 * A status whose table entry below is nonzero is a channel event: it takes
 * that many data bytes (the first is the held byte itself under running
 * status) and goes to SD_DispatchSequenceChannelEvent. Otherwise a meta or
 * SysEx status goes to its handler and anything else to
 * SD_IgnoreSequenceEvent. */
s32 SD_ReadSequenceEvent(SDSequenceTrack *track);

/* Data-byte count per status high nibble, sixteen words read out of the
 * retail image: 0 for 0x0-0x7 and 0xF, 2 for 0x8-0xB (note off/on,
 * polyphonic pressure, control change) and 0xE (pitch bend), 1 for 0xC-0xD
 * (program change, channel pressure). SD_ReadSequenceEvent is its only
 * reader. */
extern s32 gSD_adwStatusDataByteCounts[16];

#endif
