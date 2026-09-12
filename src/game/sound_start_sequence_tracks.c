/*
 * SD_StartSequenceTracks: counter lifetime and count snapshot
 *
 * The 208-byte sequence initializer at `0x8004C77C` matches all 52 instructions
 * under the existing uniform `gcc_2_8_1_g0` profile, using the shared
 * `SDSecondaryState` and `SDSequenceTrack` layouts. It removes all six register
 * bindings and the count barrier without introducing a new compiler profile,
 * declaration view, or literal global address.
 *
 * The single-iteration counter initialization preserves the retail `$s1`
 * counter and `$s2` byte-offset allocation. Flattening that scope exchanges the
 * registers in six words. The separate count-read scope keeps the bound load
 * before the counter increment; omitting both scopes leaves seven differences.
 * Array indexing and loop-local declarations alone do not change the exchanged
 * allocation.
 *
 * The function retains the initial flag resets and header read, track-position
 * save, variable-length delta read, four byte resets, pending-delta and halfword
 * stores, conditional scaling, and state/count reload on each iteration.
 * The definition remains `s32(void)`. Both existing void-returning playback call
 * views, including the state-argument view and the no-argument alias, stay
 * unchanged in `sound_sequence_timing.h`.
 *
 * The canonical match and six-row refinement series ending in deferral remain
 * intact. One post-terminal result records the newly verified source scopes.
 */
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
