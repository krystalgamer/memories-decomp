/*
 * The secondary (MIDI) sequencer's track walk, 0x8004C560-0x8004CB0C: opening
 * a track at its MTrk chunk, rescaling a pending delta to the runtime tick,
 * starting every track, aging the secondary objects' reuse counters, and the
 * per-tick track step, reset and end-of-sequence status. All of them work on
 * gSD_pSecondaryState and its SDSequenceTrack array. SD_StartSequenceTracks
 * and SD_ProcessSequenceTracks both call SD_ScaleSequenceDelta, and both
 * sequencer drivers follow their SD_ProcessSequenceTracks steps with
 * SD_UpdateSecondaryObjectReuseCountdowns.
 */
#include "../types.h"
#include "sound.h"
#include "sound_sequence_reader.h"
#include "sound_sequence_timing.h"
#include "sound_sequence_parser.h"
#include "sd_read_sequence_event.h"

#define SD_SEQUENCE_TRACK_OFFSET_VIEW(state, offset) \
    ((SDSequenceTrack *)((u8 *)(state) + (offset)))

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
   runtime tick, carrying the remainder in delta_scale_remainder. */
void SD_ScaleSequenceDelta(SDSequenceTrack *entry)
{
    switch (gSD_pSecondaryState->timebase) {
    case 0x30:
        entry->delta_remaining = entry->delta_remaining * 10;
        entry->delta_remaining = entry->delta_remaining + entry->delta_scale_remainder;
        entry->delta_scale_remainder = entry->delta_remaining & 3;
        entry->delta_remaining = entry->delta_remaining >> 2;
        return;
    case 0x60:
        entry->delta_remaining = entry->delta_remaining * 5;
        entry->delta_remaining = entry->delta_remaining + entry->delta_scale_remainder;
        entry->delta_scale_remainder = entry->delta_remaining & 3;
        entry->delta_remaining = entry->delta_remaining >> 2;
        return;
    case 0xC0:
    case 0xF0:
        entry->delta_remaining = entry->delta_remaining + entry->delta_scale_remainder;
        entry->delta_scale_remainder = entry->delta_remaining & 1;
        entry->delta_remaining = entry->delta_remaining >> 1;
        return;
    case 0x120:
    case 0x168:
        entry->delta_remaining = entry->delta_remaining / 3;
        return;
    case 0x180:
    case 0x1E0:
        entry->delta_remaining = entry->delta_remaining + entry->delta_scale_remainder;
        entry->delta_scale_remainder = entry->delta_remaining & 3;
        entry->delta_remaining = entry->delta_remaining >> 2;
        return;
    case 0x300:
    case 0x3C0:
        entry->delta_remaining = entry->delta_remaining + entry->delta_scale_remainder;
        entry->delta_scale_remainder = entry->delta_remaining & 7;
        entry->delta_remaining = entry->delta_remaining >> 3;
        return;
    default:
        return;
    }
}

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

s32 SD_StartSequenceTracks(void)
{
    SDSecondaryState *initial = gSD_pSecondaryState;
    s32 i;
    s32 offset;

    initial->delta_scale_gate = 0;
    initial->field_0800 = 0;
    SD_ReadSequenceHeader();
    {
        SDSecondaryState *state = gSD_pSecondaryState;
        if (state->track_count != 0) {
            s32 count;

            do {
                i = 0;
            } while (0);
            offset = SD_SEQUENCE_TRACK_ARRAY_OFFSET;
            do {
                SDSequenceTrack *entry =
                    SD_SEQUENCE_TRACK_OFFSET_VIEW(state, offset);
                s32 value = entry->pos;

                entry->pos_saved = value;
                value = SD_ReadVariableLengthValue(entry);
                entry->ended = 0;
                entry->field_0027 = 0;
                entry->running_status = 0;
                entry->running_status_held = 0;
                {
                    SDSecondaryState *flags_state = gSD_pSecondaryState;
                    entry->delta_remaining = value;
                    entry->delta_scale_remainder = 0;
                    if (flags_state->delta_scale_gate != 0) {
                        SD_ScaleSequenceDelta(entry);
                    }
                }
                state = gSD_pSecondaryState;
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

/* Saturates the reuse-ranking counter at zero; out-of-range channels clear
   only that counter, not the object or its channel ownership. */
void SD_UpdateSecondaryObjectReuseCountdowns(void) {
    s32 i;

    for (i = 0; i < gSD_pSecondaryState->object_count; i++) {
        SDSecondaryObject *item = &gSD_pSecondaryState->objects[i];
        if (item->reuse_countdown != 0 && item->channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
            item->reuse_countdown -= 1;
        } else {
            item->reuse_countdown = 0;
        }
    }
}

#define SD_SEQUENCE_TRACK_BYTES(state) ((u8 *)(state)->tracks)
#define SD_SEQUENCE_TRACK_VIEW(bytes) ((SDSequenceTrack *)(bytes))
#define SD_SECONDARY_STATE_NEXT_TRACK(state) \
    ((SDSecondaryState *)((u8 *)(state) + SD_SEQUENCE_TRACK_RECORD_SIZE))

/* Advances every MIDI track by one runtime tick. Each track carries a
   fixed-point tempo accumulator: tempo_step is added to
   tempo_accumulator, and a carry out of the low byte is one sequencer
   tick, at which point the track's delta countdown runs down and the next
   event is dispatched through SD_ReadSequenceEvent. */
int SD_ProcessSequenceTracks(void) {
    /* Retail keeps only the track-array base live here and reaches the four
       neighbouring state words off it, so those four stay as displacements
       from the array: addressing them through gSD_pSecondaryState costs a second
       live register and does not match. STATE_WORD spells them as their
       real SDSecondaryState offsets. Every track field below is typed. */
#define STATE_WORD(type, off)     (*(type *)(base + (off) - SD_SEQUENCE_TRACK_ARRAY_OFFSET))
    u8 *base = SD_SEQUENCE_TRACK_BYTES(gSD_pSecondaryState);
    int i;
    SDSequenceTrack *entry;

    if (STATE_WORD(s16, 0x7E2) != 1) /* sequence_state */
        return 0;
    if (STATE_WORD(u16, 0x7FA) == 0) /* track_count */
        return 0;
    i = 0;
    entry = SD_SEQUENCE_TRACK_VIEW(base);
loop:
    if (entry->ended == 0) {
        int sum = entry->tempo_accumulator + entry->tempo_step;
        entry->tempo_accumulator = sum;
        if ((u16)sum >= 0x100) {
            entry->tempo_accumulator = (u8)sum;
            if (i == STATE_WORD(u16, 0x7F8)) /* field_07F8 */
                STATE_WORD(s32, 0x7F0)++;   /* sequence_tick_count */
            {
                u32 count = entry->delta_remaining;
                if (count != 0 && gSD_pSecondaryState->fast_forward_target_address == 0) {
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
                    if (gSD_pSecondaryState->delta_scale_gate != 0)
                        SD_ScaleSequenceDelta(entry);
                    if (entry->delta_remaining == 0)
                        goto retry;
                }
                if (entry->delta_remaining != 0)
                    entry->delta_remaining--;
            }
accumulate:
            {
                SDSecondaryState *root = gSD_pSecondaryState;
                int total = (int)root->sequence_read_base + entry->pos;
                int threshold = root->fast_forward_target_address;

                root->last_track_cursor_address = total;
                if (threshold != 0 && (unsigned int)total >=
                                      (unsigned int)threshold)
                    root->fast_forward_target_address = 0;
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

    for (i = 0; i < gSD_pSecondaryState->track_count; i++) {
        gSD_pSecondaryState->tracks[i].ended = 1;
        gSD_pSecondaryState->tracks[i].pos = 0;
    }
}

/* 3 once every track has run off the end of its chunk, 1 otherwise. */
s32 SD_GetSequenceStatus(void)
{
    SDSecondaryState *object = gSD_pSecondaryState;
    s32 index;
    u16 count = object->track_count;

    for (
        index = 0;
        index < count;
        index++, object = SD_SECONDARY_STATE_NEXT_TRACK(object)
    ) {
        if (object->tracks[0].ended != 1) {
            return 1;
        }
    }

    return 3;
}
