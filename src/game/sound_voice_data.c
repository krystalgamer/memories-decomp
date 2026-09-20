#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_mix.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "sound_voice_data.h"

/* Sets the level and pan of every live voice playing one sound id. Bit
 * 0x8000 in `id` sets the CD volume instead; an id carrying the lookup tag is
 * first translated through the bank table at field_044C and field_043C.
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
void func_80048A28(id, value, word)
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
        u16 v;

        lo = (id & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = id & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        v = *(u16 *)((u8 *)a + (lo + hi) + SD_VOICE_LOOKUP_BYTE_OFFSET);
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->field_043C[v];
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

                    v = c->field_0444[vid].volume;
                    v *= left;
                    c->voice_volume_left[i] = v;
                }
                if ((u16)(word + 0x80) < 0x80) {
                    SDValue *d = g_SDValue;
                    s32 v;

                    v = d->field_0444[vid].volume;
                    v *= right;
                    d->voice_volume_right[i] = v;
                }
            }
            func_80047864(i);
        }
    }
    if (++i < SD_VOICE_SLOT_COUNT) {
        goto loop;
    }
}

void func_80048C0C(u16 value, u8 enabled)
{
    s32 i;
    s32 mask;

    if (enabled == 0)
        return;
    mask = 1;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        SDValue *state = g_SDValue;
        if (state->voice_ids[i] == value &&
            (state->voice_active_mask & mask)) {
            state->voice_step[i] = enabled;
        }
    }
}

void func_80048C70(u32 *dst, u32 *src)
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
    func_80048C70((u32 *)blk, src);
    func_8004763C();

    for (i = 0; i < 2; i++) {
        cur = other;
        if (i != 0) {
            cur = blk;
        }
        for (j = 0; j < cur->count; j++) {
            u16 key = cur->keys[j];
            if (key != SD_PENDING_ENTRY_NONE) {
                u16 n = g_SDValue->field_0440;
                u16 v;

                g_SDValue->field_043C[key] = n;
                g_SDValue->field_0444[n] = cur->data[j];
                e = (SDNote *)(
                    (u32)&((SDNote *)0)[n] +
                    (u32)g_SDValue->field_0444
                );
                v = e->field_0006;
                e->field_0006 =
                    (i != 0) ? (u16)(v + (addr_side >> 4))
                             : (u16)(v + (addr_other >> 4));
                g_SDValue->field_0440 = g_SDValue->field_0440 + 1;
            }
            if (i != 0) {
                g_SDValue->field_044C[side][j] = key;
            }
        }
    }
}

#include "sound_init.h"

/* Sound driver initialisation after the SPU is up: enables reverb in studio-A
 * mode at full depth, clears and seeds the driver's level and track fields,
 * points the music track at its buffer at 0x801EA800 with an empty header,
 * and finishes through func_80049594, func_80049600 and
 * SD_EnableSecondarySequenceUpdates.
 * sd_init_state.c calls it once. */
void func_80048F14(void)
{
    SpuReverbAttr packet;
    /* g_SDValue is reloaded three times, as retail does. The first load gets
       its own pointer so each lives only as long as its stores. */
    SDValue *a;
    SDValue *b;
    SDValue *c;

    SpuReserveReverbWorkArea(SPU_ON);
    SpuSetReverb(SPU_ON);
    packet.mask = SPU_REV_MODE | SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    packet.mode = SPU_REV_MODE_STUDIO_A;
    packet.depth.left = 0x7FFF;
    packet.depth.right = 0x7FFF;
    SpuSetReverbModeParam(&packet);
    b = g_SDValue;
    b->field_1586 = 0;
    b->field_1588 = 0;
    b->field_158A = 0;
    a = g_SDValue;
    a->field_1580 = 0xFF;
    a->field_1584 = 0xFF;
    c = g_SDValue;
    a->field_1582 = 0;
    c->field_1578 = -1;
    c->field_157A = -1;
    c->field_157C = -1;
    c->field_157E = -1;
    c->music_track = (u16 *)0x801EA800;
    c->field_1560 = (u8 *)0x801E2000;
    c->music_track[0] = 0xFFFF;
    c->music_track[1] = 0;
    *(s32 *)&c->music_track[2] = 0;
    *(s32 *)&c->music_track[4] = 0;
    *(s32 *)&c->music_track[6] = 0x40000;
    func_80049594(2);
    func_80049600(0x14);
    SD_EnableSecondarySequenceUpdates();
}
