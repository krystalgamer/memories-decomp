#include "../types.h"
#include "sound.h"
#include "sound_sequence_marker_scan.h"
#include "sound_sequence_reader.h"
#include "sound_sequence_timing.h"
#include "sound_sequence_parser.h"

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

