#include "../types.h"
#include "sound.h"
#include "sound_sequence_reader.h"
#include "sound_sequence_timing.h"
#include "sound_sequence_parser.h"

s32 SD_StartSequenceTracks(void)
{
    SDSecondaryState *initial = D_8009B458;
    s32 i;
    s32 offset;

    initial->field_0804 = 0;
    initial->field_0800 = 0;
    SD_ReadSequenceHeader();
    {
        SDSecondaryState *state = D_8009B458;
        if (state->track_count != 0) {
            s32 count;

            do {
                i = 0;
            } while (0);
            offset = SD_SEQUENCE_TRACK_ARRAY_OFFSET;
            do {
                SDSequenceTrack *entry =
                    (SDSequenceTrack *)((u8 *)state + offset);
                s32 value = entry->pos;

                entry->pos_saved = value;
                value = SD_ReadVariableLengthValue(entry);
                entry->ended = 0;
                entry->field_0027 = 0;
                entry->running_status = 0;
                entry->running_status_held = 0;
                {
                    SDSecondaryState *flags_state = D_8009B458;
                    entry->delta_remaining = value;
                    entry->field_0018 = 0;
                    if (flags_state->field_0804 != 0) {
                        SD_ScaleSequenceDelta(entry);
                    }
                }
                state = D_8009B458;
                do {
                    count = state->track_count;
                } while (0);
                i++;
                offset += SD_SEQUENCE_TRACK_RECORD_SIZE;
            } while (i < count);
        }
    }
    return 0;
}
