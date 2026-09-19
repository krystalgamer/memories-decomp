/*
 * Starts one secondary-driver note by walking Psy-Q VAB program/tone data,
 * allocating a voice, filling SpuVoiceAttr, applying pitch/spatial volume,
 * keying on, and routing reverb. Current best under gcc_2_8_1_cc_g8_as_g0:
 * 355/355 instructions, an EMPTY opcode census, and 305 of 355 aligned on
 * opcode and registers in 23 structural blocks, with no register binding.
 *
 * READ THE CENSUS FROM THE ENCODED FIELDS ON THIS FUNCTION. align_functions.py
 * compares rendered mnemonics and reports `move +1, addu -1, sll +1, nop -1`,
 * a distance of 4. Decoded from bits 31-26 with SPECIAL taking funct, the
 * census is empty: SPECIAL:21 (every addu) is 37 against 37 and SPECIAL:00
 * (every sll, nop included) is 59 against 59. What exists is one register
 * difference -- objdump prints `addu rd,rs,$zero` as `move`, and the target
 * has 17 of those against 16 -- and one operand difference, a real sll where
 * this source has a nop. Two operand differences counted as four opcode
 * deltas. The distance-2 figure the previous header quoted has the same
 * origin.
 *
 * Unsigned key/tone indices, uncached driver-root loads, block-scoped reverb
 * masks, folded VAB indices, two allocation call sites, and one forced root
 * reload reproduce the current shape. The incoming channel keeps retail's
 * callee-saved register because its name is reused after its last use, first
 * for tone[3] and then as the pitch shift count.
 *
 * Three levers, each measured alone and in combination against the state
 * below it, and they compose:
 *  - tone[2] is read inline at its single use rather than through a `tr`
 *    local (296 -> 301 aligned, 26 -> 25 structural);
 *  - the two stores of `level` are one chained assignment,
 *    `obj[6] = obj[5] = level;` (301 -> 304, 25 -> 23). BOTH ORDERS TIE
 *    EXACTLY here, which is worth knowing because the chain's order is
 *    usually load-bearing; `obj[5] = obj[6] = level;` is the same figures.
 *  - the key's upper bound is tested against the tone record's address
 *    expression, matching the lower bound beside it (304 -> 305).
 *
 * MEASURED AND CLOSED: reading tone[6] through the `tone` pointer instead of
 * the repeated address expression is 282 aligned over 23 structural blocks
 * AND BREAKS THE EMPTY CENSUS (distance 2). The repeated expression is
 * deliberate. Extending the same spelling to the vag read is 280 over 38
 * with census 14, and moving the `tone` assignment down to its first use is
 * 289 over 29. Only the [7] site pays.
 *
 * decomp-permuter, rerun after refreshing its in-tree base, produced 34
 * outputs; all were re-scored by splicing each body into the real source.
 * Seven beat the installed state on the project's key. Its best, at 301/25,
 * decomposes to the tone[2] lever alone -- the pointer local and the
 * `->`/`(*x).` rewrite it also carried are worth nothing. The chained
 * assignment came from two other outputs.
 *
 * Residual: register choices only.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound_sequence_constants.h"

/* Second name for the same symbol: retail re-reads the pointer here rather
   than reusing the base it just stored through. */
#define SDSECONDARYSTATE_AS_BYTES
#define SDSECONDARYSTATE_RELOAD_ALIAS
#include "../game/sound.h"
#include "../game/sound_secondary_object_selection.h"

#include "../game/func_80049FB4.h"
#include "../game/sound_spatialize_object.h"
#include "../game/sound_spatialization.h"

void func_8004ADE8(s32 arg0, s32 note, u8 velocity)
{
    s32 channel;
    u8 *prog;
    s32 ch;
    u32 tidx;
    u32 key;
    u8 *rec;
    u8 *vab;
    u8 *hdr;
    u8 *tone;
    u8 *obj;
    s32 idx;
    s32 used;
    s32 i;
    s32 sum;
    s32 vag;
    u16 *sizes;
    u8 program;
    u8 stolen;
    s32 level;
    s32 pitch;
    s32 amode;
    s32 *voice;
    u16 adsr1;
    u16 adsr2;
    u8 tr;

    channel = arg0;
    rec = D_8009B458 + (channel & 0xFF) * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
    hdr = *(u8 **)(D_8009B458 + 0x4A8);
    program = rec[0];
    vab = hdr;
    used = 0;
    i = 0;
    if (program != 0) {
        do {
            if (vab[i * 0x10 + 0x20] != 0) {
                used++;
            }
            i++;
        } while (i < program);
    }
    prog = &vab[program * 0x10 + 0x20];
    if (prog[0] == 0) {
        return;
    }
    tidx = 0;
    key = note & 0xFF;
    ch = channel & 0xFF;
    level = note & 0x7F;
    do {
        tone = &vab[(used * 16 + (tidx & 0xFFFF)) * 32 + 0x820];
        if (key < (&vab[(used * 16 + (tidx & 0xFFFF)) * 32 + 0x820])[6]) {
            goto next;
        }
        if ((&vab[(used * 16 + (tidx & 0xFFFF)) * 32 + 0x820])[7] < key) {
            goto next;
        }
        {
            u32 limit = rec[6] & 0xF0;

            stolen = 0;
            if (limit != 0) {
                if ((rec[6] & 0xF) >= (limit >> 4)) {
                    idx = SD_FindLowestPrioritySecondaryObject(ch);
                    if (idx == -1) {
                        idx = SD_SelectSecondaryObject(ch, key);
                        goto have;
                    }
                    stolen = 1;
                    goto have;
                }
            }
        }
        idx = SD_SelectSecondaryObject(channel & 0xFF, key);
    have:
        if (idx == -1) {
            goto next;
        }
        i = 0;
        sum = 0;
        vag = *(s16 *)(tone + 0x16);
        sizes = (u16 *)(*(u8 **)(D_8009B458 + 0x4A8)
                        + (*(u16 *)(hdr + 0x12)) * 512 + 0x820);
        obj = &D_8009B458[idx * SD_SECONDARY_OBJECT_SIZE + 0x180];
        if (vag > 0) {
            do {
                sum += sizes[i];
                i++;
            } while (i < vag);
        }
        sum <<= 3;
        voice = &D_80011434[idx];
        *(s32 *)(D_8009B458 + 0x4C4) = 0x6019F;
        *(s16 *)(D_8009B458 + 0x4CC) = 0;
        *(s16 *)(D_8009B458 + 0x4CE) = 0;
        *(s32 *)(D_8009B458 + 0x4C0) = *voice;
        *(s32 *)(D_8009B458 + 0x4DC) = *(s32 *)(D_8009B458 + 0x4B8) + sum;
        adsr1 = *(u16 *)(tone + 0x10);
        *(s16 *)(D_8009B458 + 0x4FA) = adsr1;
        *(u16 *)(obj + 0x20) = adsr1;
        adsr2 = *(u16 *)(tone + 0x12);
        *(s16 *)(D_8009B458 + 0x4FC) = adsr2;
        *(u16 *)(obj + 0x22) = adsr2;
        if ((*(u16 *)(tone + 0x10) & 0x80) == 0) {
            amode = 1;
        } else {
            amode = 5;
        }
        *(s32 *)(D_8009B458 + 0x4E4) = amode;
        *(u16 *)(obj + 0x24) = *(u16 *)(D_8009B458_r + 0x4E4);
        rec[4] = velocity;
        obj[0x12] = tone[4];
        obj[0x13] = tone[5];
        obj[0x11] = tone[0xC];
        obj[0] = idx;
        obj[4] = tidx;
        obj[2] = program;
        obj[6] = obj[5] = level;
        obj[3] = channel;
        obj[0xD] = 1;
        obj[0x10] = tone[0xD];
        ((SDSecondaryObject *)obj)->field_0008 = prog[1];
        ((SDSecondaryObject *)obj)->field_000A = prog[4];
        ((SDSecondaryObject *)obj)->field_0009 = tone[2];
        ((SDSecondaryObject *)obj)->field_000E = velocity;
        *(u16 *)(obj + 0x1E) = 0xFFFF;
        channel = tone[3];
        ((SDSecondaryObject *)obj)->field_000B = channel;
        SD_SpatializeSecondaryObject(
            (SDSecondaryObject *)(D_8009B458 + idx * SD_SECONDARY_OBJECT_SIZE + 0x180),
            (SDSecondaryRecord *)(D_8009B458 + ch * SD_SEQUENCE_CHANNEL_RECORD_SIZE));
        *(s16 *)(D_8009B458 + 0x4C8) =
            (((SDSecondaryObject *)obj)->level_left *
             *(u16 *)(D_8009B458 + 0x514)) >> 7;
        *(s16 *)(D_8009B458 + 0x4CA) =
            (((SDSecondaryObject *)obj)->level_right *
             *(u16 *)(D_8009B458 + 0x516)) >> 7;
        obj[5] = level;
        *(s16 *)(obj + 0x1A) = -1;
        *(s16 *)(obj + 0x1C) = rec[7];
        pitch = SD_CalcPitchBend((SDSecondaryObject *)obj, rec[7]) + obj[6] * 128;
        channel = 7;
        *(s16 *)(D_8009B458 + 0x4D4) =
            func_80049FB4((s16)pitch >> channel, pitch & 0x7F, tone[4], tone[5]);
        SpuSetKeyOnWithAttr((SpuVoiceAttr *)(D_8009B458 + 0x4C0));
        if (stolen == 0) {
            if ((rec[6] & 0xF) < 0xF) {
                rec[6] = rec[6] + 1;
            }
        }
        obj[0xF] = 1;
        if (rec[0x10] == 0) {
            if ((tone[1] & 4) != 0) {
                s32 *m = &D_80011434[idx];
                do {
                    SpuSetReverbVoice(SPU_ON, *m);
                } while ((SpuGetReverbVoice() & *m) == 0);
            } else {
                s32 *m = &D_80011434[idx];
                do {
                    SpuSetReverbVoice(SPU_OFF, *m);
                } while ((SpuGetReverbVoice() & *m) != 0);
            }
        } else if (rec[0x10] != 1) {
            s32 *m = &D_80011434[idx];
            do {
                SpuSetReverbVoice(SPU_ON, *m);
            } while ((SpuGetReverbVoice() & *m) == 0);
        } else {
            s32 *m = &D_80011434[idx];
            do {
                SpuSetReverbVoice(SPU_OFF, *m);
            } while ((SpuGetReverbVoice() & *m) != 0);
        }
    next:
        tidx++;
    } while ((tidx & 0xFFFF) < prog[0]);
}
