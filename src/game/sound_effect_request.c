#include "../types.h"
#include "sound_output_state.h"
#include "sound.h"
#include "sound_voice_allocator.h"
#include "sound_effect_request.h"
#include "../psyq/libspu.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"
#include "sound_voice_selection.h"
#include "sound_pending_entries.h"
#include "sound_init.h"

void SD_SEPlay(s32 sound_id, s32 volume, s32 pan_value)
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
       chain: GCC must not forward sound_id into the second ID snapshot. */
    if (sound_id & 0x8000) {
        tags.words.original = sound_id;
        pan = pan_value;
        sound_id = volume;
        tags.words.copy = tags.words.original;
        vol = sound_id;
        stop_value = tags.words.original;
    } else {
        tags.words.original = sound_id;
        pan = pan_value;
        sound_id = volume;
        tags.words.copy = tags.words.original;
        vol = sound_id;
        stop_value = tags.words.original;
    }
    if (tags.words.original & 0x8000) {
        SD_EnqueueCdSePlay(stop_value & 0xFFFF, 0);
        return;
    }
    if ((tags.words.original & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;
        s32 n;

        lo = (tags.words.original & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        t2 = tags.words.original & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = t2;
        hi = (hi != 0) << 6;

        table = (u8 *)&a->s_aawSD_INDIRECT_SE_ID;
        v = *(u16 *)(table + (lo + hi));
        if (v == SD_PENDING_ENTRY_NONE) {
            return;
        }
        n = a->s_pwSD_SE_INDEX[v];
        if (n == SD_PENDING_ENTRY_NONE) {
            return;
        }
        /* Array indexing changes the direct-ID path's address registers. */
        e = (SDNote *)(n * SD_NOTE_RECORD_SIZE + (u32)a->s_pstSD_SE_TABLE);
        t2 = e->priority;
        last_arg = t2;
        vol &= 0xFF;
        SD_AllocateSoundEffectVoice(v, 0, vol, (s16)pan, e->mode, last_arg & 0xFF);
    } else {
        SDValue *b = g_SDValue;

        n = b->s_pwSD_SE_INDEX[tags.words.copy & 0xFFFF];
        if (n == SD_PENDING_ENTRY_NONE) {
            return;
        }
        e = (SDNote *)(n * SD_NOTE_RECORD_SIZE + (u32)b->s_pstSD_SE_TABLE);
        t2 = e->priority;
        last_arg = t2;
        vol &= 0xFF;
        SD_AllocateSoundEffectVoice(tags.words.copy & 0xFFFF, 0, vol, (s16)pan, e->mode, last_arg & 0xFF);
    }
}

void SD_SESetPan(s32 sound_id, s32 pan_value)
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

    key = sound_id;
    pan2 = pan_value;
    if (sound_id & 0x8000) {
        SD_SetCdPan((s16)pan_value);
        return;
    }
    if ((sound_id & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        off = (sound_id & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        flag = sound_id & SD_VOICE_LOOKUP_BANK_FLAG;
        flag = 0 < flag;
        off += flag << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        base = (u8 *)g_SDValue;
        /* Keep the state base first in the sum to preserve retail's addu order. */
        pt = *(u16 *)(base + off + (u32)&((SDValue *)0)->s_aawSD_INDIRECT_SE_ID);
        sound_id = SD_PENDING_ENTRY_NONE;
        if (pt == sound_id) {
            return;
        }
        key = ((SDValue *)base)->s_pwSD_SE_INDEX[pt];
        if (key == sound_id) {
            return;
        }
    }
    i = 0;
    nt = (u16)((u32)pan_value - 1);
    near = nt < 0x80;
    pan = (s16)pan_value;
    k = 0x80;
    lo = k - pan;
    hi = pan + k;
    /* Entered through a constant switch so loop.c leaves the voice loop
       alone; see SD_SESetVolumePan below. */
    switch (0) {
        for (;;) {
        }
        do {
    case 0:
            SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, env);
            v = g_SDValue->s_awSD_VOICE_SE_ID[i];
            if (v == (u16)key) {
                if (env[0] != 0) {
                    if (pan != 0) {
                        if (near) {
                            prod = g_SDValue->s_pstSD_SE_TABLE[v].volume;
                            prod *= lo;
                            g_SDValue->s_awSD_VOICE_VOLUME_L[i] = prod;
                        }
                        if ((u16)(pan2 + 0x80) < 0x80) {
                            prod = g_SDValue->s_pstSD_SE_TABLE[v].volume;
                            prod *= hi;
                            g_SDValue->s_awSD_VOICE_VOLUME_R[i] = prod;
                        }
                    }
                    SD_ApplyVoiceSlotVolume(i);
                }
            }
            i++;
        } while (i < SD_VOICE_SLOT_COUNT);
    }
}

void SD_SESetVolume(s32 sound_id, s32 volume)
{
    s32 lo;
    s32 hi;
    s32 i;
    u8 a1v = volume;
    s32 idm;
    u16 id;
    s32 ff;

    id = sound_id;
    if (sound_id & 0x8000) {
        SD_SetCdVolume((u8)a1v);
        return;
    }
    if ((sound_id & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;

        lo = (sound_id & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = sound_id & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        /* Keep the table base separate from the byte offset for allocation. */
        table = (u8 *)&a->s_aawSD_INDIRECT_SE_ID;
        v = *(u16 *)(table + (lo + hi));
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->s_pwSD_SE_INDEX[v];
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
        if (b->s_awSD_VOICE_SE_ID[i] == idm && local != 0) {
            b->s_abSD_VOICE_FADE_LEVEL[i] = a1v;
            SD_ApplyVoiceSlotVolume(i);
        }
    } while (++i < SD_VOICE_SLOT_COUNT);
}

/* Sets the level and pan of every live voice playing one sound id. Bit
 * 0x8000 in `id` sets the CD volume instead; an id carrying the lookup tag is
 * first translated through the bank table at s_aawSD_INDIRECT_SE_ID and s_pwSD_SE_INDEX.
 * `word` is a signed pan offset: left and right get 0x80 minus and plus it,
 * each only when it stays inside the byte range.
 *
 * Three spellings are load bearing, each measured against the retail words:
 *
 * - The definition is old style with u16/u8/u16 parameters. The callers and
 *   sound_effect_request.h still see three words, which old-style promotion keeps
 *   compatible, but inside the function GCC keeps each incoming register and
 *   a narrowed copy apart. That is retail's shape: the early tests read
 *   $a0/$a2 and the voice loop reads the copies in $s2/$s3/$s7.
 * - Retail recomputes i * 2 every pass of the voice loop, which loop.c would
 *   have strength-reduced in an ordinary structured loop. The constant switch
 *   enters the do/while at its case label; until flow deletes the dead empty
 *   loop that jump comes from outside the loop, so loop.c leaves it alone,
 *   and jump2 deletes the jump afterwards.
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
        g_SDValue->s_wSD_CD_VOLUME = value;
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
        table = (u8 *)&a->s_aawSD_INDIRECT_SE_ID;
        v = *(u16 *)(table + (lo + hi));
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->s_pwSD_SE_INDEX[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    leftOk = (u16)(word - 1) < 0x80;
    pan = (s16)word;
    left = 0x80 - pan;
    right = pan + 0x80;
    switch (0) {
        for (;;) {
        }
        do {
    case 0:
            {
                s16 local;
                SDValue *b;
                u16 vid;

                SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
                b = g_SDValue;
                vid = b->s_awSD_VOICE_SE_ID[i];
                if (vid == (id & 0xFFFF) && local != 0) {
                    b->s_abSD_VOICE_FADE_LEVEL[i] = value;
                    if (pan != 0) {
                        if (leftOk) {
                            SDValue *c = g_SDValue;
                            s32 v;

                            v = c->s_pstSD_SE_TABLE[vid].volume;
                            v *= left;
                            c->s_awSD_VOICE_VOLUME_L[i] = v;
                        }
                        if ((u16)(word + 0x80) < 0x80) {
                            SDValue *d = g_SDValue;
                            s32 v;

                            v = d->s_pstSD_SE_TABLE[vid].volume;
                            v *= right;
                            d->s_awSD_VOICE_VOLUME_R[i] = v;
                        }
                    }
                    SD_ApplyVoiceSlotVolume(i);
                }
            }
        } while (++i < SD_VOICE_SLOT_COUNT);
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
        if (state->s_awSD_VOICE_SE_ID[i] == id &&
            (state->s_bSD_VOICE_ACTIVE_MASK & mask)) {
            state->s_abSD_VOICE_FADE_STEP[i] = step;
        }
    }
}

void SD_CopySeqBlock(u32 *dst, u32 *src)
{
    u32 i;

    for (i = 0; i < SD_PENDING_INPUT_COPY_GROUP_COUNT; i++) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}

void SD_LoadSequenceBankPair(s32 side, u32 *src)
{
    SDSeqBlock *blk;
    SDSeqBlock *other;
    SDSeqBlock *cur;
    SDNote *e;
    u32 addr_side;
    u32 addr_other;
    s32 rev;
    s32 i;
    s32 j;

    if ((u32)side >= 2) {
        return;
    }

    rev = 1 - side;
    addr_side = 0xD810 + (side % 2) * 0x19000;
    addr_other = 0xD810 + (rev % 2) * 0x19000;
    blk = (SDSeqBlock *)(0x801E7800 + (side << SD_PENDING_INPUT_BLOCK_SHIFT));
    other = (SDSeqBlock *)(0x801E7800 + (rev << SD_PENDING_INPUT_BLOCK_SHIFT));
    SD_CopySeqBlock((u32 *)blk, src);
    SD_ResetPendingEntries();

    for (i = 0; i < 2; i++) {
        cur = other;
        if (i != 0) {
            cur = blk;
        }
        for (j = 0; j < cur->count; j++) {
            u16 key = cur->keys[j];
            if (key != SD_PENDING_ENTRY_NONE) {
                u16 n = g_SDValue->s_wSD_NEXT_SE_INDEX;
                u16 v;

                g_SDValue->s_pwSD_SE_INDEX[key] = n;
                g_SDValue->s_pstSD_SE_TABLE[n] = cur->data[j];
                e = (SDNote *)(
                    (u32)&((SDNote *)0)[n] +
                    (u32)g_SDValue->s_pstSD_SE_TABLE
                );
                v = e->spu_address_div16;
                e->spu_address_div16 =
                    (i != 0) ? (u16)(v + (addr_side >> 4))
                             : (u16)(v + (addr_other >> 4));
                g_SDValue->s_wSD_NEXT_SE_INDEX = g_SDValue->s_wSD_NEXT_SE_INDEX + 1;
            }
            if (i != 0) {
                g_SDValue->s_aawSD_INDIRECT_SE_ID[side][j] = key;
            }
        }
    }
}

/* Sound driver initialisation after the SPU is up: enables reverb in studio-A
 * mode at full depth, clears and seeds the driver's level and track fields,
 * points the music track at its buffer at 0x801EA800 with an empty header,
 * and finishes through SD_SetSequenceTickMode, SD_SetSecondaryObjectCount and
 * SD_EnableSecondarySequenceUpdates.
 * sound_driver.c (SD_InitState) calls it once. */
void SD_InitSecondaryRuntime(void)
{
    SpuReverbAttr reverb_attr;
    /* g_SDValue is reloaded three times, as retail does. The first load gets
       its own pointer so each lives only as long as its stores. */
    SDValue *bgm_state;
    SDValue *sequence_state;
    SDValue *music_state;

    SpuReserveReverbWorkArea(SPU_ON);
    SpuSetReverb(SPU_ON);
    reverb_attr.mask = SPU_REV_MODE | SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    reverb_attr.mode = SPU_REV_MODE_STUDIO_A;
    reverb_attr.depth.left = 0x7FFF;
    reverb_attr.depth.right = 0x7FFF;
    SpuSetReverbModeParam(&reverb_attr);
    sequence_state = g_SDValue;
    sequence_state->s_wSD_SEQ_FADE_LEVEL = 0;
    sequence_state->s_wSD_SEQ_FADE_STEP = 0;
    sequence_state->s_bSD_SEQ_FADE_TARGET = 0;
    bgm_state = g_SDValue;
    bgm_state->s_wSD_BGM_FADE_LEVEL = 0xFF;
    bgm_state->s_bSD_BGM_FADE_TARGET = 0xFF;
    music_state = g_SDValue;
    bgm_state->s_wSD_BGM_FADE_STEP = 0;
    music_state->s_wSD_OPEN_MUSIC_PACKAGE = -1;
    music_state->s_wSD_MUSIC_VAB_ID = -1;
    music_state->s_wSD_OPEN_SEQUENCE_COMMAND = -1;
    music_state->s_wSD_SEQ_ACCESS = -1;
    music_state->s_pwSD_MUSIC_TRACK = (u16 *)0x801EA800;
    music_state->s_pstSD_MUSIC_TABLE = (SDMusicTableEntry *)0x801E2000;
    music_state->s_pwSD_MUSIC_TRACK[0] = 0xFFFF;
    music_state->s_pwSD_MUSIC_TRACK[1] = 0;
    *(s32 *)&music_state->s_pwSD_MUSIC_TRACK[2] = 0;
    *(s32 *)&music_state->s_pwSD_MUSIC_TRACK[4] = 0;
    *(s32 *)&music_state->s_pwSD_MUSIC_TRACK[6] = 0x40000;
    SD_SetSequenceTickMode(2);
    SD_SetSecondaryObjectCount(0x14);
    SD_EnableSecondarySequenceUpdates();
}

/* Retires the driver's two pending requests, in the same *g_SDValue struct
   whose s_wSD_BGM_FADE_STEP/s_bSD_BGM_FADE_TARGET SD_StartBgmFadeCopy and SD_StartBgmFade
   write.

   If the 0x157E slot is active, it stops that sequence through SD_StopSequence
   when the 0x80 flag bit is set (clearing the bit), releases the slot through
   SD_CloseSequence, and marks it inactive. Separately, if the VAB slot at 0x157A
   is ready for release, it resets that VAB transfer state and marks
   0x157A/0x1578 inactive. Always zeroes 0x1586/0x1588. */

void SD_ResetMusicState(void) {
    if (g_SDValue->s_wSD_SEQ_ACCESS != -1) {
        if (g_SDValue->s_wSD_STATUS & 0x80) {
            SD_StopSequence(g_SDValue->s_wSD_SEQ_ACCESS);
            g_SDValue->s_wSD_STATUS =
                g_SDValue->s_wSD_STATUS & 0xFF7F;
        }
        SD_CloseSequence(g_SDValue->s_wSD_SEQ_ACCESS);
        g_SDValue->s_wSD_SEQ_ACCESS = -1;
    }

    if (g_SDValue->s_wSD_MUSIC_VAB_ID == 0) {
        SD_ResetVabTransferState(0);
        g_SDValue->s_wSD_MUSIC_VAB_ID = -1;
        g_SDValue->s_wSD_OPEN_MUSIC_PACKAGE = -1;
    }

    g_SDValue->s_wSD_SEQ_FADE_STEP = 0;
    g_SDValue->s_wSD_SEQ_FADE_LEVEL = 0;
}

void SD_StartBgmFadeCopy(s16 value, u8 flag)
{
    g_SDValue->s_wSD_BGM_FADE_STEP = value;
    g_SDValue->s_bSD_BGM_FADE_TARGET = flag;
}

void SD_StartBgmFade(s16 step, u8 target)
{
    g_SDValue->s_wSD_BGM_FADE_STEP = step;
    g_SDValue->s_bSD_BGM_FADE_TARGET = target;
}

s32 SD_IsBgmFadeActive(void)
{
    return g_SDValue->s_wSD_BGM_FADE_STEP != 0;
}
