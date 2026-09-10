#ifndef MEMORIES_DECOMP_SD_READ_SEQUENCE_EVENT_H
#define MEMORIES_DECOMP_SD_READ_SEQUENCE_EVENT_H

#include "../types.h"
#include "sound_sequence_parser.h"

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
 * The definition pins several locals to named registers. That spelling is
 * load bearing and should not be relaxed into ordinary locals without a
 * measurement.
 *
 * It sits in its own header rather than in sound.h because the sequence
 * reader family is spelled that way: SD_ReadSequenceByte lives in
 * sound_sequence_reader.h, and the two big-endian readers and
 * SD_ReadSequenceHeader in sound_sequence_parser.h. */
s32 SD_ReadSequenceEvent(SDSequenceTrack *arg0);

#endif
