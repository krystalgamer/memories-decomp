#include "../types.h"
#include "../psyq/libapi.h"

#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_sequence_events.h"
#include "sound_sequence_parser.h"

/* The secondary sequencer's stream reader, in image order: closing and
   stopping the root-counter timer that sound_sequence_events.c opens, the
   per-tick SD_UpdateSecondarySequence, the byte and variable-length readers,
   SD_FindMidiTrackChunk (whose "MTrk" literal is the unit's .sdata),
   the big-endian readers and header reader, the meta, SysEx and channel event
   handlers, and SD_ReadSequenceEvent, the running-status reader that
   dispatches to them. Everything consumes an SDSequenceTrack stream through
   SD_ReadSequenceByte; sound_sequence_tracks.c walks the tracks with it. */

void SD_CloseSequenceTimerEvent(void)
{
    gSD_pSecondaryState->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    DisableEvent(gSD_pSecondaryState->event_handle);
    CloseEvent(gSD_pSecondaryState->event_handle);
    ExitCriticalSection();
    gSD_pSecondaryState->timer_update_inhibit = 0;
}

void SD_StopSequenceTimer(void)
{
    gSD_pSecondaryState->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    ExitCriticalSection();
    gSD_pSecondaryState->timer_update_inhibit = 0;
}

void SD_UpdateSecondarySequence(void)
{
    if (gSD_pSecondaryState->update_in_progress)
        return;
    gSD_pSecondaryState->update_in_progress = 1;
    if (gSD_pSecondaryState->shared_track_updates_enabled) {
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
    }
    SD_UpdateSecondaryObjectReuseCountdowns();
    SD_UpdateSecondaryObjectVoices();
    gSD_pSecondaryState->update_in_progress = 0;
}

/* Compares count bytes; returns the difference at the first mismatch. */
s32 SD_CompareBytes(u8 *left, u8 *right, s32 count) {
    if (count == 0) {
        return -1;
    }

    while (--count != 0 && *left == *right) {
        left++;
        right++;
    }

    return *left - *right;
}

s32 SD_ReadSequenceByte(SDSequenceTrack *reader)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    int offset = reader->pos;
    int value = state->sequence_read_base[offset];

    offset++;
    reader->pos = offset;
    if ((u32)state->sequence_read_limit < (u32)offset) {
        reader->ended = 1;
        return -1;
    }
    return value;
}

s32 SD_ReadVariableLengthValue(SDSequenceTrack *input)
{
    int value = SD_ReadSequenceByte(input);
    int result;

    if (value == 0) {
        return 0;
    }
    if (value == SD_SEQUENCE_VLQ_INITIAL_STOP) {
        input->ended = 1;
        return 0;
    }
    result = value;
    if (result & SD_SEQUENCE_VLQ_CONTINUATION_BIT) {
        result &= SD_SEQUENCE_VLQ_PAYLOAD_MASK;
        do {
            value = SD_ReadSequenceByte(input);
            result = (result << SD_SEQUENCE_VLQ_PAYLOAD_BITS) +
                     (value & SD_SEQUENCE_VLQ_PAYLOAD_MASK);
        } while (value & SD_SEQUENCE_VLQ_CONTINUATION_BIT);
    }
    return result;
}

#define SD_SEQUENCE_MARKER_SIZE 4

s32 SD_FindMidiTrackChunk(s32 offset)
{
    do {
        if (SD_CompareBytes(
                (u8 *)"MTrk",
                gSD_pSecondaryState->sequence_read_base + offset,
                SD_SEQUENCE_MARKER_SIZE) == 0) {
            return offset + SD_SEQUENCE_MARKER_SIZE;
        }

        offset++;
    } while ((u32)gSD_pSecondaryState->sequence_read_limit >= (u32)offset);

    return -1;
}

#define SD_SECONDARY_STATE_AT_OFFSET(state, offset) \
    ((SDSecondaryState *)((u8 *)(state) + (offset)))

s32 SD_ReadSequenceU32BE(SDSequenceTrack *input)
{
    s32 first = SD_ReadSequenceByte(input);
    s32 second = SD_ReadSequenceByte(input);
    s32 third = SD_ReadSequenceByte(input);
    s32 fourth = SD_ReadSequenceByte(input);

    return (fourth & 0xFF) + ((third & 0xFF) << 8) +
           ((second & 0xFF) << 16) + (first << 24);
}

s32 SD_ReadSequenceU16BE(SDSequenceTrack *input)
{
    s32 high = SD_ReadSequenceByte(input);
    s32 low = SD_ReadSequenceByte(input);

    return (low & 0xFF) | ((high & 0xFF) << 8);
}

/* MATCH (2026-09-05). Was an ASSEMBLY TRANSCRIPTION (Unchiga's port of
 * 2026-08-30, an inline asm block) counted as debt in docs/ASM_DEBT.md;
 * this is the C. Flags: -O2 -G0 -mno-split-addresses, as -G0 (gp == 0, the
 * scalar gSD_pSecondaryState arm). The full history is in docs/PARKED.txt.
 *
 * Track/tempo-stream setup on gSD_pSecondaryState: resets the record's stream
 * fields, reads a halfword through SD_ReadSequenceU16BE and a word through
 * SD_ReadSequenceU32BE (its top three bytes are the raw tempo), computes
 * 60000000 / tempo * 100 / 115, clamps it to 0xFF, halves or quarters it by
 * mode (0x1E: >> 2; 0x18 and 0x3C: >> 1) and stores the byte into +0x16 and
 * +0x14 of the record before handing it to SD_ReadSequenceByte.
 *
 * The shape is the gFile_dwTransferState-family idiom: a `do { } while (0);` (a macro
 * in the original) round the eight statements from the +0x7F0 store to the
 * +0x808 store. The last three levers, in order: a base local `b` loaded
 * BEFORE the byte store through the pointee, so the +0x518 store goes
 * through the pre-store base while everything after the `sb` reloads;
 * the dividend named inside the pin (`k = 60000000;`); the call result
 * routed through the later result name (`v = call >> 8; r = v;`), which
 * puts the whole reciprocal block in retail's order; and finally the
 * +0x7F0 store moved INSIDE the pin, which is what forms `p` and copies it
 * into $a0 before the two word stores -- the permuter found that last one
 * at score 0 after 13 -> 7 by hand. Every one of these was measured as
 * worthless or worse while an earlier fault was still open.
 */

s32 SD_ReadSequenceHeader(void) {
    SDSecondaryState *b;
    SDSequenceTrack *p;
    u32 r;
    u32 v;
    s32 m;
    u32 k;

    b = gSD_pSecondaryState;
    p = b->tracks;
    b->field_0801 = 0;
    do {
        gSD_pSecondaryState->sequence_tick_count = 0;
        gSD_pSecondaryState->sequence_tick_count_saved = 0;
        b->tracks[0].pos = 8;
        gSD_pSecondaryState->timebase = SD_ReadSequenceU16BE(p);
        gSD_pSecondaryState->track_count = 1;
        gSD_pSecondaryState->field_07F8 = 0;
        gSD_pSecondaryState->sequence_read_limit = 0x10000;
        v = (u32)SD_ReadSequenceU32BE(p) >> 8;
        r = v;
        k = 60000000;
        gSD_pSecondaryState->tempo_us_per_quarter = r;
    } while (0);

    v = k / r;
    v = v * 100 / 115;
    if (v >= 0x100) {
        v = 0xFF;
    }

    m = gSD_pSecondaryState->timebase;
    switch (m) {
    case 0x18:
    case 0x3C:
        v >>= 1;
        break;
    case 0x1E:
        v >>= 2;
        break;
    }
    p->tempo_step = v & 0xFF;
    p->tempo_accumulator = v & 0xFF;
    SD_ReadSequenceByte(p);

    if (gSD_pSecondaryState->timebase >= 0x60) {
        gSD_pSecondaryState->delta_scale_gate = gSD_pSecondaryState->timebase;
    } else {
        gSD_pSecondaryState->delta_scale_gate = 0;
    }
    gSD_pSecondaryState->delta_scale_gate = gSD_pSecondaryState->timebase;
    return 1;
}

void SD_AdvanceSequencePosition(s32 *value, s32 amount)
{
    *value += amount;
}

void SD_IgnoreSequenceEvent(u8 status)
{
}

void SD_HandleSequenceMetaEvent(SDSequenceTrack *p, s32 meta_type)
{
    SDSecondaryState *e;
    SDSecondaryState *f;
    u32 v;
    u32 i;
    s32 mode;
    s32 off;
    s32 t;
    s32 z;

    mode = meta_type & 0xFF;

    switch (mode) {
    case SD_SEQUENCE_META_END_OF_TRACK:
        p->ended = 1;
        SD_ReadSequenceByte(p);
        break;

    case SD_SEQUENCE_META_TEMPO:
        z = 0;
        v = SD_ReadSequenceByte(p) << 16;
        v = v | (SD_ReadSequenceByte(p) << 8);
        v = v | SD_ReadSequenceByte(p);
        e = gSD_pSecondaryState;
        e->tempo_us_per_quarter = v;
        v = 0x3938700 / v;
        v = v * 100 / 115;
        if (v >= 0x100) {
            v = 0xFF;
        }
        t = e->timebase;
        switch (t) {
        case 0x3C:
        case 0x18:
            v = v >> 1;
            break;
        case 0x1E:
            v = v >> 2;
            break;
        }
        f = gSD_pSecondaryState;
        if (f->track_count != 0) {
            /* Retail recomputes the record address as `state + off` on
               every iteration, so the byte offset has to stay: walking
               `f->tracks[i]` strength-reduces to a pointer and loses the
               add. Re-basing the state view keeps the field names. */
            off = z;
            i = z;
            do {
                SDSecondaryState *at =
                    SD_SECONDARY_STATE_AT_OFFSET(f, off);

                at->tracks[0].tempo_step = v;
                at->tracks[0].tempo_accumulator = v;
                off += SD_SEQUENCE_TRACK_RECORD_SIZE;
                i++;
            } while (i < f->track_count);
        }
        break;

    case SD_SEQUENCE_META_SMPTE_OFFSET:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        break;

    case SD_SEQUENCE_META_TIME_SIGNATURE:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceU32BE(p);
        break;

    case SD_SEQUENCE_META_KEY_SIGNATURE:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        break;

    case 0 ... 0xF:
    default:
        SD_AdvanceSequencePosition(&p->pos, SD_ReadVariableLengthValue(p));
        break;
    }
}

void SD_SkipSequenceSysEx(SDSequenceTrack *input)
{
    unsigned int i = 0;
    unsigned int count = SD_ReadVariableLengthValue(input);

    do {
        i++;
        if ((u8)SD_ReadSequenceByte(input) == SD_SEQUENCE_SYSEX_END)
            break;
    } while (i < count);
}

/* Sequence channel event dispatcher: routes a note off (0x80), note on
   (0x90, a zero velocity is a note off), controller (0xB0), program (0xC0)
   or pitch (0xE0) message to the secondary-object handlers. Controller
   0x63 with value 0x14 saves every track's position and registers (or
   just this track's when the sequence has no track table) for a loop, and
   value 0x1E counts the loop down and restores them; controller 6 while
   the channel is in mode 0x14 sets the loop count on every track (or on
   this track alone, without forwarding) before the controller is passed
   on. */
void SD_DispatchSequenceChannelEvent(SDSequenceTrack *p, s32 status, u8 d1, u8 d2) {
    SDSequenceTrack *t;
    u8 ch;
    s32 i;
    u8 v;

    ch = status & SD_SEQUENCE_CHANNEL_MASK;
    switch (status & SD_SEQUENCE_STATUS_MASK) {
    case SD_SEQUENCE_NOTE_OFF:
        SD_KeyOffSecondaryNote(ch, d1, d2);
        break;
    case SD_SEQUENCE_NOTE_ON:
        if (d2 != 0) {
            SD_KeyOnSecondaryNote(ch, d1, d2);
        } else {
            SD_KeyOffSecondaryNote(ch, d1, 0);
        }
        break;
    case SD_SEQUENCE_CONTROL_CHANGE:
        if (d1 == SD_SEQUENCE_CONTROL_MODE) {
            if (d2 == SD_SEQUENCE_LOOP_START) {
                SDSecondaryState *seq;

                seq = gSD_pSecondaryState;
                if (seq->field_07F8 != 0) {
                    if (seq->track_count != 0) {
                        i = 0;
                        do {
                            t = &seq->tracks[i];
                            t->ended_saved = t->ended;
                            t->running_status_saved = t->running_status;
                            t->loop_count = d2;
                            t->pos_saved = t->pos;
                            t->delta_remaining_saved = t->delta_remaining;
                            t->delta_scale_remainder_saved = t->delta_scale_remainder;
                            seq = gSD_pSecondaryState;
                            i++;
                        } while (i < seq->track_count);
                    }
                    gSD_pSecondaryState->sequence_tick_count_saved = gSD_pSecondaryState->sequence_tick_count;
                } else {
                    p->loop_count = SD_SEQUENCE_LOOP_UNCOUNTED;
                    p->pos_saved = p->pos;
                }
            } else if (d2 == SD_SEQUENCE_LOOP_END) {
                v = p->loop_count;
                if (v != 0) {
                    SDSecondaryState *seq;

                    if (v < SD_SEQUENCE_LOOP_UNCOUNTED) {
                        p->loop_count = v - 1;
                    }
                    seq = gSD_pSecondaryState;
                    if (seq->field_07F8 != 0) {
                        if (seq->track_count != 0) {
                            i = 0;
                            do {
                                t = &seq->tracks[i];
                                t->ended = t->ended_saved;
                                t->running_status = t->running_status_saved;
                                t->pos = t->pos_saved;
                                t->delta_remaining = t->delta_remaining_saved;
                                t->delta_scale_remainder = t->delta_scale_remainder_saved;
                                seq = gSD_pSecondaryState;
                                i++;
                            } while (i < seq->track_count);
                        }
                        gSD_pSecondaryState->sequence_tick_count = gSD_pSecondaryState->sequence_tick_count_saved;
                    } else {
                        p->pos = p->pos_saved;
                    }
                }
            }
        }
        if (d1 == SD_SEQUENCE_CONTROL_DATA_ENTRY) {
            SDSecondaryState *seq;

            seq = gSD_pSecondaryState;
            if (seq->channels[ch].control_mode == SD_SEQUENCE_LOOP_START) {
                if (seq->field_07F8 != 0) {
                    if (seq->track_count != 0) {
                        i = 0;
                        do {
                            seq->tracks[i].loop_count = d2;
                            seq = gSD_pSecondaryState;
                            i++;
                        } while (i < seq->track_count);
                    }
                } else {
                    p->loop_count = d2;
                    break;
                }
            }
        }
        SD_HandleSequenceControlChange(ch, d1, d2);
        break;
    case SD_SEQUENCE_PROGRAM_CHANGE:
        SD_SetSequenceChannelProgram(ch, d1);
        break;
    case SD_SEQUENCE_PITCH_BEND:
        SD_SetSequenceChannelPitchBendMsb(ch, d1, d2);
        break;
    }
}

/* Data-byte count per status high nibble, the unit's only read-only data. */
const s32 gSD_adwStatusDataByteCounts[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 1, 1, 2, 0
};

/* The event byte is kept twice: `cmd` as read, and `ev` as a u8 copy. GCC
 * 2.8.1 assigns a u8 local with a plain move and zero-extends at each use,
 * which is retail's `$a1` to `$a0` copy followed by `andi 0xFF` at the meta
 * test and in the tail. `byte2` is declared before `type` so the two
 * equal-priority callee-saved pseudos keep retail's $s2/$s3 order. */
s32 SD_ReadSequenceEvent(SDSequenceTrack *track)
{
    s32 cmd;
    u8 ev;
    u32 status;
    u8 byte2;
    s32 type;
    u8 byte3;

    byte3 = 0;
    cmd = SD_ReadSequenceByte(track);
    ev = cmd;
    if (!(cmd & SD_SEQUENCE_STATUS_BIT)) {
        status = track->running_status;
        track->running_status_held = 1;
    } else {
        if (ev != SD_SEQUENCE_META_EVENT) {
            track->running_status = cmd;
        }
        status = cmd;
        track->running_status_held = 0;
    }
    type = gSD_adwStatusDataByteCounts[(status >> SD_SEQUENCE_STATUS_SHIFT) &
                      SD_SEQUENCE_STATUS_INDEX_MASK];
    if (type != 0) {
        if (track->running_status_held == 0) {
            byte2 = SD_ReadSequenceByte(track);
        } else {
            byte2 = ev;
        }
        if (type == 2) {
            byte3 = SD_ReadSequenceByte(track);
        }
        SD_DispatchSequenceChannelEvent(track, status & 0xFF, byte2, byte3);
        return 0;
    }
    switch (ev) {
    case SD_SEQUENCE_META_EVENT:
        SD_HandleSequenceMetaEvent(track, SD_ReadSequenceByte(track) & 0xFF);
        return 0;
    case SD_SEQUENCE_SYSEX_EVENT:
        SD_SkipSequenceSysEx(track);
        return 0;
    default:
        SD_IgnoreSequenceEvent(ev);
        return 0;
    }
}
