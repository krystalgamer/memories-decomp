#include "../types.h"

#include "sound.h"
#include "sound_sequence_timing.h"
#include "sound_sequence_reader.h"
#include "sd_read_sequence_event.h"

/* Advances every MIDI track by one runtime tick. Each track carries a
   fixed-point tempo accumulator: tempo_step is added to
   tempo_accumulator, and a carry out of the low byte is one sequencer
   tick, at which point the track's delta countdown runs down and the next
   event is dispatched through SD_ReadSequenceEvent. */
int SD_ProcessSequenceTracks(void) {
    /* Retail keeps only the track-array base live here and reaches the four
       neighbouring state words off it, so those four stay as displacements
       from the array: addressing them through D_8009B458 costs a second
       live register and does not match. STATE_WORD spells them as their
       real SDSecondaryState offsets. Every track field below is typed. */
#define STATE_WORD(type, off)     (*(type *)(base + (off) - SD_SEQUENCE_TRACK_ARRAY_OFFSET))
    u8 *base = (u8 *)D_8009B458->tracks;
    int i;
    SDSequenceTrack *entry;

    if (STATE_WORD(s16, 0x7E2) != 1) /* field_07E2 */
        return 0;
    if (STATE_WORD(u16, 0x7FA) == 0) /* track_count */
        return 0;
    i = 0;
    entry = (SDSequenceTrack *)base;
loop:
    if (entry->ended == 0) {
        int sum = entry->tempo_accumulator + entry->tempo_step;
        entry->tempo_accumulator = sum;
        if ((u16)sum >= 0x100) {
            entry->tempo_accumulator = (u8)sum;
            if (i == STATE_WORD(u16, 0x7F8)) /* field_07F8 */
                STATE_WORD(s32, 0x7F0)++;   /* field_07F0 */
            {
                u32 count = entry->delta_remaining;
                if (count != 0 && D_8009B458->field_080C == 0) {
                    entry->delta_remaining = count - 1;
                    goto accumulate;
                }
retry:
                SD_ReadSequenceEvent(entry);
                if (entry->ended == 0) {
                    int value = SD_ReadVariableLengthValue(entry);
                    entry->delta_remaining = value;
                    if (value == 0)
                        goto retry;
                    if (D_8009B458->field_0804 != 0)
                        SD_ScaleSequenceDelta(entry);
                    if (entry->delta_remaining == 0)
                        goto retry;
                }
                if (entry->delta_remaining != 0)
                    entry->delta_remaining--;
            }
accumulate:
            {
                SDSecondaryState *root = D_8009B458;
                int total = (int)root->field_07DC + entry->pos;
                int threshold = root->field_080C;

                root->field_0810 = total;
                if (threshold != 0 && (unsigned int)total >=
                                      (unsigned int)threshold)
                    root->field_080C = 0;
            }
        }
    }
    {
        int count = STATE_WORD(u16, 0x7FA); /* track_count */
        i++;
        if (i < count) {
            entry++;
            goto loop;
        }
    }
    return 0;
#undef STATE_WORD
}

void SD_ResetSequenceTracks(void) {
    s32 i;

    for (i = 0; i < D_8009B458->track_count; i++) {
        D_8009B458->tracks[i].ended = 1;
        D_8009B458->tracks[i].pos = 0;
    }
}

/* 3 once every track has run off the end of its chunk, 1 otherwise. */
s32 SD_GetSequenceStatus(void)
{
    SDSecondaryState *object = D_8009B458;
    s32 index;
    u16 count = object->track_count;

    for (
        index = 0;
        index < count;
        index++, object = (SDSecondaryState *)((u8 *)object +
                                               SD_SEQUENCE_TRACK_RECORD_SIZE)
    ) {
        if (object->tracks[0].ended != 1) {
            return 1;
        }
    }

    return 3;
}
