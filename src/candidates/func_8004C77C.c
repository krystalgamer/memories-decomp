/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 6 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_sequence_timing.c.
 */
#include "../types.h"
#include "../game/sound.h"
#include "../game/sound_sequence_marker_scan.h"
#include "../game/sound_sequence_reader.h"
#include "../game/sound_sequence_timing.h"
#include "../game/sound_sequence_parser.h"

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
