#include "../types.h"
#include "sound.h"
#include "sound_sequence_marker_scan.h"
#include "sound_sequence_reader.h"
#include "sound_sequence_values.h"

int SD_OpenSequenceTrack(SDSequenceTrack *entry)
{
    entry->pos = SD_FindMidiTrackChunk(entry->pos);
    if (entry->pos == -1)
        return 1;
    entry->chunk_length = SD_ReadSequenceU32BE(entry);
    entry->chunk_start = entry->pos;
    entry->chunk_end = entry->pos + entry->chunk_length;
    return 0;
}

/* Rescales the track's delta count from the sequence's own timebase to the
   runtime tick, carrying the remainder in field_0018. */
void SD_ScaleSequenceDelta(SDSequenceTrack *entry)
{
    switch (D_8009B458->timebase) {
    case 0x30:
        entry->delta_remaining = entry->delta_remaining * 10;
        entry->delta_remaining = entry->delta_remaining + entry->field_0018;
        entry->field_0018 = entry->delta_remaining & 3;
        entry->delta_remaining = entry->delta_remaining >> 2;
        return;
    case 0x60:
        entry->delta_remaining = entry->delta_remaining * 5;
        entry->delta_remaining = entry->delta_remaining + entry->field_0018;
        entry->field_0018 = entry->delta_remaining & 3;
        entry->delta_remaining = entry->delta_remaining >> 2;
        return;
    case 0xC0:
    case 0xF0:
        entry->delta_remaining = entry->delta_remaining + entry->field_0018;
        entry->field_0018 = entry->delta_remaining & 1;
        entry->delta_remaining = entry->delta_remaining >> 1;
        return;
    case 0x120:
    case 0x168:
        entry->delta_remaining = entry->delta_remaining / 3;
        return;
    case 0x180:
    case 0x1E0:
        entry->delta_remaining = entry->delta_remaining + entry->field_0018;
        entry->field_0018 = entry->delta_remaining & 3;
        entry->delta_remaining = entry->delta_remaining >> 2;
        return;
    case 0x300:
    case 0x3C0:
        entry->delta_remaining = entry->delta_remaining + entry->field_0018;
        entry->field_0018 = entry->delta_remaining & 7;
        entry->delta_remaining = entry->delta_remaining >> 3;
        return;
    default:
        return;
    }
}

extern void SD_ReadSequenceHeader(void);

int SD_StartSequenceTracks(void)
{
    register SDSecondaryState *initial asm("$2") = D_8009B458;
    register int i asm("$17");
    register int offset asm("$18");

    initial->field_0804 = 0;
    initial->field_0800 = 0;
    SD_ReadSequenceHeader();
    {
        register SDSecondaryState *state asm("$4") = D_8009B458;
        if (state->track_count != 0) {
            int count;

            i = 0;
            offset = SD_SEQUENCE_TRACK_ARRAY_OFFSET;
            do {
                register SDSequenceTrack *entry asm("$16") =
                    (SDSequenceTrack *)((u8 *)state + offset);
                int value = entry->pos;

                entry->pos_saved = value;
                value = SD_ReadVariableLengthValue(entry);
                entry->ended = 0;
                entry->field_0027 = 0;
                entry->running_status = 0;
                entry->running_status_held = 0;
                {
                    register SDSecondaryState *flags_state asm("$3") =
                        D_8009B458;
                    entry->delta_remaining = value;
                    entry->field_0018 = 0;
                    if (flags_state->field_0804 != 0) {
                        SD_ScaleSequenceDelta(entry);
                    }
                }
                state = D_8009B458;
                count = state->track_count;
                asm volatile("" : "+r"(count));
                i++;
                offset += SD_SEQUENCE_TRACK_RECORD_SIZE;
            } while (i < count);
        }
    }
    return 0;
}
