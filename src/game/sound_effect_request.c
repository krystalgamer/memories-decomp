#include "../types.h"
#include "sound_output_state.h"
#include "sound.h"
#include "sound_voice_allocator.h"
#include "sound_effect_request.h"
#include "../psyq/libspu.h"
#include "sound_mix.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"
#include "sound_voice_selection.h"
#include "sound_voice_pan.h"
#include "sd_set_cd_volume.h"
#include "sound_pending_entries.h"
#include "sound_voice_data.h"

void SD_SEPlay(s32 arg0, s32 arg1, s32 arg2)
{
    SDSEPlayIdPair tags;
    s32 vol;
    s32 pan;
    u8 last_arg;
    u16 stop_value;
    s32 lo;
    s32 hi;
    s32 n;
    SDNote *e;
    s32 t2;

    /* The duplicate paths and consumed-id reuse preserve the retail copy
       chain: GCC must not forward arg0 into the second ID snapshot. */
    if (arg0 & 0x8000) {
        tags.words.original = arg0;
        pan = arg2;
        arg0 = arg1;
        tags.words.copy = tags.words.original;
        vol = arg0;
        stop_value = tags.words.original;
    } else {
        tags.words.original = arg0;
        pan = arg2;
        arg0 = arg1;
        tags.words.copy = tags.words.original;
        vol = arg0;
        stop_value = tags.words.original;
    }
    if (tags.words.original & 0x8000) {
        SD_EnqueueCdSePlay(stop_value & 0xFFFF, 0);
        return;
    }
    if ((tags.words.original & 0xF000) == 0x4000) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;
        s32 n;

        lo = (tags.words.original & 0x1F) << 1;
        t2 = tags.words.original & 0x100;
        hi = t2;
        hi = (hi != 0) << 6;

        table = (u8 *)&a->field_044C;
        v = *(u16 *)(table + (lo + hi));
        if (v == 0xFFFF) {
            return;
        }
        n = a->note_index_by_id[v];
        if (n == 0xFFFF) {
            return;
        }
        /* Array indexing changes the direct-ID path's address registers. */
        e = (SDNote *)(n * SD_NOTE_RECORD_SIZE + (u32)a->note_table);
        t2 = e->priority;
        last_arg = t2;
        vol &= 0xFF;
        SD_AllocateSoundEffectVoice(v, 0, vol, (s16)pan, e->mode, last_arg & 0xFF);
    } else {
        SDValue *b = g_SDValue;

        n = b->note_index_by_id[tags.words.copy & 0xFFFF];
        if (n == 0xFFFF) {
            return;
        }
        e = (SDNote *)(n * SD_NOTE_RECORD_SIZE + (u32)b->note_table);
        t2 = e->priority;
        last_arg = t2;
        vol &= 0xFF;
        SD_AllocateSoundEffectVoice(tags.words.copy & 0xFFFF, 0, vol, (s16)pan, e->mode, last_arg & 0xFF);
    }
}

void SD_SESetPan(s32 arg0, s32 arg1)
{
    s32 i;
    s32 pan;
    s32 lo;
    s32 hi;
    s32 near;
    u16 key;
    s16 env[4];
    u8 *base;
    s32 v;
    s32 off;
    s16 pan2;
    u32 flag;
    s32 pt;
    s32 k;
    u32 nt;
    s32 prod;

    key = arg0;
    pan2 = arg1;
    if (arg0 & 0x8000) {
        SD_SetCdPan((s16)arg1);
        return;
    }
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        off = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        flag = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        flag = 0 < flag;
        off += flag << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        base = (u8 *)g_SDValue;
        /* Keep the state base first in the sum to preserve retail's addu order. */
        pt = *(u16 *)(base + off + (u32)&((SDValue *)0)->field_044C);
        arg0 = SD_PENDING_ENTRY_NONE;
        if (pt == arg0) {
            return;
        }
        key = ((SDValue *)base)->note_index_by_id[pt];
        if (key == arg0) {
            return;
        }
    }
    i = 0;
    nt = (u16)((u32)arg1 - 1);
    near = nt < 0x80;
    pan = (s16)arg1;
    k = 0x80;
    lo = k - pan;
    hi = pan + k;
loop:
    {
        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, env);
        v = g_SDValue->voice_ids[i];
        if (v == (u16)key) {
            if (env[0] != 0) {
                if (pan != 0) {
                    if (near) {
                        prod = g_SDValue->note_table[v].volume;
                        prod *= lo;
                        g_SDValue->voice_volume_left[i] = prod;
                    }
                    if ((u16)(pan2 + 0x80) < 0x80) {
                        prod = g_SDValue->note_table[v].volume;
                        prod *= hi;
                        g_SDValue->voice_volume_right[i] = prod;
                    }
                }
                SD_ApplyVoiceSlotVolume(i);
            }
        }
        i++;
    }
    if (i < SD_VOICE_SLOT_COUNT) {
        goto loop;
    }
}

void SD_SESetVolume(s32 arg0, s32 arg1)
{
    s32 lo;
    s32 hi;
    s32 i;
    u8 a1v = arg1;
    s32 idm;
    u16 id;
    s32 ff;

    id = arg0;
    if (arg0 & 0x8000) {
        SD_SetCdVolume((u8)a1v);
        return;
    }
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;

        lo = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        /* Keep the table base separate from the byte offset for allocation. */
        table = (u8 *)&a->field_044C;
        v = *(u16 *)(table + (lo + hi));
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->note_index_by_id[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    idm = id & 0xFFFF;
    do {
        s16 local;
        SDValue *b;

        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
        b = g_SDValue;
        if (b->voice_ids[i] == idm && local != 0) {
            b->voice_value[i] = a1v;
            SD_ApplyVoiceSlotVolume(i);
        }
    } while (++i < SD_VOICE_SLOT_COUNT);
}

/* Sets the level and pan of every live voice playing one sound id. Bit
 * 0x8000 in `id` sets the CD volume instead; an id carrying the lookup tag is
 * first translated through the bank table at field_044C and note_index_by_id.
 * `word` is a signed pan offset: left and right get 0x80 minus and plus it,
 * each only when it stays inside the byte range.
 *
 * Three spellings are load bearing, each measured against the retail words:
 *
 * - The definition is old style with u16/u8/u16 parameters. The callers and
 *   sound_voice_data.h still see three words, which old-style promotion keeps
 *   compatible, but inside the function GCC keeps each incoming register and
 *   a narrowed copy apart. That is retail's shape: the early tests read
 *   $a0/$a2 and the voice loop reads the copies in $s2/$s3/$s7.
 * - The voice loop is a goto loop: retail recomputes i * 2 every pass, which
 *   loop.c would have strength-reduced in a structured loop. The one-iteration
 *   do/while around the voice_value store is the only loop weighting left.
 *   It doubles that reference in flow's count, and without it `value` sinks
 *   below right/left/leftOk in global-alloc priority and the $s3..$s7 roles
 *   rotate.
 * - Each level product goes through one local that first holds the note
 *   volume. That gives the pseudo register uses on both sides of the mult,
 *   so it is allocated $v0 (lbu/mult/mflo $v0) instead of LO plus a reload. */
void SD_SESetVolumePan(id, value, word)
    u16 id;
    u8 value;
    u16 word;
{
    s32 i;
    s32 pan;
    s32 right;
    s32 left;
    s32 leftOk;
    s32 lo;
    s32 hi;
    s32 ff;

    if (id & 0x8000) {
        g_SDValue->cd_volume = value;
        SD_SetCdPan((s16)word);
        return;
    }
    if ((id & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;

        lo = (id & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = id & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        /* As in SD_SESetVolume, the table base stays separate from the
           byte offset so the add keeps retail's operand order. */
        table = (u8 *)&a->field_044C;
        v = *(u16 *)(table + (lo + hi));
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->note_index_by_id[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    leftOk = (u16)(word - 1) < 0x80;
    pan = (s16)word;
    left = 0x80 - pan;
    right = pan + 0x80;
loop:
    {
        s16 local;
        SDValue *b;
        u16 vid;

        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
        b = g_SDValue;
        vid = b->voice_ids[i];
        if (vid == (id & 0xFFFF) && local != 0) {
            do {
                b->voice_value[i] = value;
            } while (0);
            if (pan != 0) {
                if (leftOk) {
                    SDValue *c = g_SDValue;
                    s32 v;

                    v = c->note_table[vid].volume;
                    v *= left;
                    c->voice_volume_left[i] = v;
                }
                if ((u16)(word + 0x80) < 0x80) {
                    SDValue *d = g_SDValue;
                    s32 v;

                    v = d->note_table[vid].volume;
                    v *= right;
                    d->voice_volume_right[i] = v;
                }
            }
            SD_ApplyVoiceSlotVolume(i);
        }
    }
    if (++i < SD_VOICE_SLOT_COUNT) {
        goto loop;
    }
}

void SD_FadeOutSoundEffectVoices(u16 id, u8 step)
{
    s32 i;
    s32 mask;

    if (step == 0)
        return;
    mask = 1;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        SDValue *state = g_SDValue;
        if (state->voice_ids[i] == id &&
            (state->voice_active_mask & mask)) {
            state->voice_step[i] = step;
        }
    }
}
