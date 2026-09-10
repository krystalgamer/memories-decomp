#include "../types.h"
#include "sound.h"
#include "sound_secondary_commands.h"
#include "sound_sequence_parser.h"
#include "sound_sequence_reader.h"
#include "../unmatched.h"

/* The sequence parser: the fixed-width big-endian readers, the header reader
   that uses them to set up track and tempo, and the handlers for the meta,
   SysEx and channel events the running-status reader dispatches. All eight
   consume the same SDSequenceTrack stream through SD_ReadSequenceByte.

   SD_ReadSequenceHeader was recorded at gcc_2_8_1_cc_g8_as_g0_split against
   gcc_2_8_1_g0 for the rest; it compiles to an identical object at
   gcc_2_8_1_g0, as its own history note below says it was first matched
   with -G0. Bounded below by SD_FindMidiTrackChunk, which does need its
   profile, and above by SD_ReadSequenceEvent at gcc_2_8_1_g8_split. */

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
 * scalar D_8009B458 arm). The full history is in docs/PARKED.txt.
 *
 * Track/tempo-stream setup on D_8009B458: resets the record's stream
 * fields, reads a halfword through SD_ReadSequenceU16BE and a word through
 * SD_ReadSequenceU32BE (its top three bytes are the raw tempo), computes
 * 60000000 / tempo * 100 / 115, clamps it to 0xFF, halves or quarters it by
 * mode (0x1E: >> 2; 0x18 and 0x3C: >> 1) and stores the byte into +0x16 and
 * +0x14 of the record before handing it to SD_ReadSequenceByte.
 *
 * The shape is the D_8009B0F4-family idiom: a `do { } while (0);` (a macro
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
    u8 *b;
    SDSequenceTrack *p;
    u32 r;
    u32 v;
    s32 m;
    u32 k;

    b = (u8 *)D_8009B458;
    p = (SDSequenceTrack *)(b + SD_SEQUENCE_TRACK_ARRAY_OFFSET);
    b[0x801] = 0;
    do {
        D_8009B458->field_07F0 = 0;
        D_8009B458->field_07F4 = 0;
        ((SDSequenceTrack *)(b + SD_SEQUENCE_TRACK_ARRAY_OFFSET))->pos = 8;
        D_8009B458->timebase = SD_ReadSequenceU16BE(p);
        D_8009B458->track_count = 1;
        D_8009B458->field_07F8 = 0;
        D_8009B458->field_07EC = 0x10000;
        v = (u32)SD_ReadSequenceU32BE(p) >> 8;
        r = v;
        k = 60000000;
        D_8009B458->field_0808 = r;
    } while (0);

    v = k / r;
    v = v * 100 / 115;
    if (v >= 0x100) {
        v = 0xFF;
    }

    m = D_8009B458->timebase;
    if (m == 0x1E) {
        goto sh2;
    }
    if (m < 0x1F) {
        if (m == 0x18) {
            goto sh1;
        }
        goto store;
    }
    if (m != 0x3C) {
        goto mask;
    }
sh1:
    v >>= 1;
    goto mask;
sh2:
    v >>= 2;
mask:
store:
    p->tempo_step = v & 0xFF;
    p->tempo_accumulator = v & 0xFF;
    SD_ReadSequenceByte(p);

    if (D_8009B458->timebase >= 0x60) {
        D_8009B458->field_0804 = D_8009B458->timebase;
    } else {
        D_8009B458->field_0804 = 0;
    }
    D_8009B458->field_0804 = D_8009B458->timebase;
    return 1;
}

void SD_AdvanceSequencePosition(s32 *value, s32 amount)
{
    *value += amount;
}

void SD_IgnoreSequenceEvent(void)
{
}

void SD_HandleSequenceMetaEvent(SDSequenceTrack *p, s32 arg1)
{
    SDSecondaryState *e;
    SDSecondaryState *f;
    u32 v;
    u32 i;
    s32 mode;
    s32 off;
    s32 t;
    s32 z;

    mode = arg1 & 0xFF;

    switch (mode) {
    case SD_SEQUENCE_META_END_OF_TRACK:
        p->ended = 1;
        goto one;

    case SD_SEQUENCE_META_TEMPO:
        z = 0;
        v = SD_ReadSequenceByte(p) << 16;
        v = v | (SD_ReadSequenceByte(p) << 8);
        v = v | SD_ReadSequenceByte(p);
        e = D_8009B458;
        e->field_0808 = v;
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
        f = D_8009B458;
        if (f->track_count != 0) {
            /* Retail recomputes the record address as `state + off` on
               every iteration, so the byte offset has to stay: walking
               `f->tracks[i]` strength-reduces to a pointer and loses the
               add. Re-basing the state view keeps the field names. */
            off = z;
            i = z;
            do {
                SDSecondaryState *at = (SDSecondaryState *)((u8 *)f + off);

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
        goto three;

    case SD_SEQUENCE_META_TIME_SIGNATURE:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceU32BE(p);
        break;

    case SD_SEQUENCE_META_KEY_SIGNATURE:
    three:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
    one:
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
        func_8004B374(ch, d1, d2);
        break;
    case SD_SEQUENCE_NOTE_ON:
        if (d2 != 0) {
            func_8004ADE8(ch, d1, d2);
        } else {
            func_8004B374(ch, d1, 0);
        }
        break;
    case SD_SEQUENCE_CONTROL_CHANGE:
        if (d1 == SD_SEQUENCE_CONTROL_MODE) {
            if (d2 == SD_SEQUENCE_LOOP_START) {
                SDSecondaryState *seq;

                seq = D_8009B458;
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
                            t->field_0018_saved = t->field_0018;
                            seq = D_8009B458;
                            i++;
                        } while (i < seq->track_count);
                    }
                    D_8009B458->field_07F4 = D_8009B458->field_07F0;
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
                    seq = D_8009B458;
                    if (seq->field_07F8 != 0) {
                        if (seq->track_count != 0) {
                            i = 0;
                            do {
                                t = &seq->tracks[i];
                                t->ended = t->ended_saved;
                                t->running_status = t->running_status_saved;
                                t->pos = t->pos_saved;
                                t->delta_remaining = t->delta_remaining_saved;
                                t->field_0018 = t->field_0018_saved;
                                seq = D_8009B458;
                                i++;
                            } while (i < seq->track_count);
                        }
                        D_8009B458->field_07F0 = D_8009B458->field_07F4;
                    } else {
                        p->pos = p->pos_saved;
                    }
                }
            }
        }
        if (d1 == SD_SEQUENCE_CONTROL_DATA_ENTRY) {
            SDSecondaryState *seq;

            seq = D_8009B458;
            if (seq->channels[ch].control_mode == SD_SEQUENCE_LOOP_START) {
                if (seq->field_07F8 != 0) {
                    if (seq->track_count != 0) {
                        i = 0;
                        do {
                            seq->tracks[i].loop_count = d2;
                            seq = D_8009B458;
                            i++;
                        } while (i < seq->track_count);
                    }
                } else {
                    p->loop_count = d2;
                    break;
                }
            }
        }
        func_8004B49C(ch, d1, d2);
        break;
    case SD_SEQUENCE_PROGRAM_CHANGE:
        func_8004B6E8(ch, d1);
        break;
    case SD_SEQUENCE_PITCH_BEND:
        func_8004B70C(ch, d1, d2);
        break;
    }
}
