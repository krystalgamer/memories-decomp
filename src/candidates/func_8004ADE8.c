/*
 * Starts one secondary-driver note by walking Psy-Q VAB program/tone data,
 * allocating a voice, filling SpuVoiceAttr, applying pitch/spatial volume,
 * keying on, and routing reverb. Current best under gcc_2_8_1_cc_g8_as_g0:
 * 355/355 instructions at exact length, an EMPTY opcode census by encoded
 * fields (bits 31-26, SPECIAL by funct), 294 of 355 aligned on opcode and
 * registers in 13 structural blocks, 75 raw words differing with relocations
 * masked, 28 with the register fields masked as well, and a shift-aware
 * structural distance of 25 (difflib over the register-masked words). The
 * previous state was 355, 21 structural blocks, 305 aligned, 153 raw, 137
 * register-masked, shift-aware 71, with a census that was empty only by
 * cancellation: it recomputed the tone pointer at word 72 where retail has a
 * nop (one addu too many) and lacked retail's copy of the loop bound at
 * word 121 (one addu too few). Its header said "register choices only";
 * read by raw words it was six zones of order and layout.
 *
 * Read the census from the encoded fields on this function: the aligner
 * compares rendered mnemonics and reports `move` against `addu` and `nop`
 * against `sll`. And read the residue by raw words, not by the aligner
 * (#5358 erases the a0-a3 names).
 *
 * Levers, each measured alone against the state below it and then combined:
 *  - the spilled locals are declared `used`, `program`, `vab`, `rec` (in
 *    that order, after `prog`): spill slots follow declaration order and
 *    retail's frame is used at 20(sp), program at 24, vab at 32, rec at 36.
 *    Five orders measured; only this one and its rec/vab swap reach it
 *    (raw 153 -> 142, shift-aware 71 -> 59);
 *  - the spatialize call's first argument carries its offset in a local,
 *    `soff = idx * SD_SECONDARY_OBJECT_SIZE + 0x180;` assigned after the
 *    tone[5] store and added to the root at the call: retail computes the
 *    offset early, interleaved with the byte stores, and adds the reloaded
 *    root last (shift-aware 59 -> 54). Passing `obj` itself is -2, and a
 *    local holding the whole address is +5;
 *  - `vag` is an `s16` local: retail copies the loop bound into a0 before
 *    the size loop (`move a0,a1`, word 121) and the s32 local has no copy.
 *    A second name for the bound and a re-read of the tone field measure
 *    identically; the narrow type is the plain spelling;
 *  - `*(s16 *)(obj + 0x1C) = rec[7];` is written BEFORE `+0x1A = -1`:
 *    retail loads rec[7] first and fills its delay slot with the -1. Alone,
 *    each of these two is one instruction off in opposite directions (+1
 *    and -1) and reads as worse on every count; together they are 142 ->
 *    75 raw, 125 -> 48 register-masked, 54 -> 40;
 *  - `td = tone[0xD];` (a u8) read right after the `obj[0x11] = tone[0xC]`
 *    store and stored into obj[0x10] later (permuter find): retail batches
 *    the two tone loads and keeps the second store where the source has it
 *    (75 -> 63 raw, 48 -> 36, 40 -> 32). Naming tone[0xC] as well, or both
 *    before the stores, is worse;
 *  - `nb = *(s32 *)(D_8009B458 + 0x4B8);` read before the `*voice` store
 *    and added to `sum` after it (permuter find): the load is issued ahead
 *    of the store group as retail does (63 -> 60, 36 -> 34, 32 -> 30);
 *  - `level = note & 0x7F;` inside the tone loop rather than before it
 *    (permuter find): register-only, 60 -> 52 raw, 313 -> 320 aligned;
 *  - the key tests read `tone[6]` and `tone[7]` through the `tone` pointer
 *    computed above them, not through the repeated address expression:
 *    retail computes the pointer once into s2 (words 66-67) and word 72 is
 *    a nop where the repeated expression recomputes it. This is what closes
 *    the census (register-masked 34 -> 31, shift-aware 30 -> 27). It costs
 *    the allocation of two callee-saved registers: with the pointer live
 *    from the key tests on, `tone` takes s1 and `idx` s2, the reverse of
 *    retail, at every one of their uses (raw 52 -> 71). The earlier header
 *    called this spelling "worse and census-breaking" on the aligner's
 *    mnemonic census; on the encoded census it is the opposite;
 *  - the pitch shift count is the literal 7, not `channel = 7;` reused: the
 *    reuse materialised the 7 into channel's callee-saved register in a
 *    branch delay slot where retail has `li v0,1` (shift-aware 27 -> 25,
 *    register-masked 31 -> 28, four register-only words);
 *  - from before: unsigned key/tone indices, uncached driver-root loads,
 *    block-scoped reverb masks, folded VAB indices, two allocation call
 *    sites, one forced root reload, tone[2] read inline at its single use,
 *    `obj[6] = obj[5] = level;` as one chained assignment (both orders tie),
 *    and the key's upper bound tested against the record like the lower.
 *
 * MEASURED AND DEAD on this base: `idx` declared before `hdr`, first of all
 * locals, or `tone` declared last (the s1/s2 swap does not move); a named
 * 0x820 shared by the tone address and the sizes address (identical to the
 * pointer spelling); `tone` given a birth before the loop (nothing); the
 * tone[3] reuse of `channel` removed (mixed), and both `channel` reuses
 * removed together (+4 instructions); D_80011434 as `const` with const
 * pointers (nothing) or read inline instead of through `voice` (-1); the
 * amode as a ternary, inline or with the branch flipped (nothing or worse);
 * a named -1 for the idx test (register-only, +1 raw); `used = 0; i = 0;`
 * in the other order or at the function top (nothing). The velocity spill
 * store and the `used = 0` store are emitted in the other order (words
 * 18-19) from every spelling measured.
 *
 * Residual, 75 words: `tone`/`idx` in s1/s2 where retail has s2/s1, at
 * every use (about twenty register-only words); the two parameters in
 * s4/s7 where retail has s7/s4 (the frame saves follow; `key` computed first
 * is -1); in the SpuVoiceAttr block retail materialises 0x6019F in two
 * halves around the loads (`lui` first, `ori` right before the store) and
 * shifts `sum` first, this source stores the constant first; the program
 * spill is reloaded before the tidx store where retail loads it after; the
 * tone[2]/tone[3]/velocity/0xFFFF stores are emitted in another order; and
 * the amode constant is in v1 where retail has v0. See
 * config/slus_01411/candidates.json.
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
    s32 used;
    u8 program;
    u8 *vab;
    u8 *rec;
    s32 ch;
    u32 tidx;
    u32 key;
    u8 *hdr;
    u8 *tone;
    u8 *obj;
    s32 idx;
    s32 i;
    s32 sum;
    s16 vag;
    u16 *sizes;
    u8 stolen;
    s32 level;
    s32 pitch;
    s32 amode;
    s32 *voice;
    s32 soff;
    u16 adsr1;
    u16 adsr2;
    u8 tr;
    s32 nb;
    u8 td;

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
    do {
        level = note & 0x7F;
        tone = &vab[(used * 16 + (tidx & 0xFFFF)) * 32 + 0x820];
        if (key < tone[6]) {
            goto next;
        }
        if (tone[7] < key) {
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
        nb = *(s32 *)(D_8009B458 + 0x4B8);
        *(s32 *)(D_8009B458 + 0x4C0) = *voice;
        *(s32 *)(D_8009B458 + 0x4DC) = nb + sum;
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
        soff = idx * SD_SECONDARY_OBJECT_SIZE + 0x180;
        obj[0x11] = tone[0xC];
        td = tone[0xD];
        obj[0] = idx;
        obj[4] = tidx;
        obj[2] = program;
        obj[6] = obj[5] = level;
        obj[3] = channel;
        obj[0xD] = 1;
        obj[0x10] = td;
        ((SDSecondaryObject *)obj)->field_0008 = prog[1];
        ((SDSecondaryObject *)obj)->field_000A = prog[4];
        ((SDSecondaryObject *)obj)->field_0009 = tone[2];
        ((SDSecondaryObject *)obj)->field_000E = velocity;
        *(u16 *)(obj + 0x1E) = 0xFFFF;
        channel = tone[3];
        ((SDSecondaryObject *)obj)->field_000B = channel;
        SD_SpatializeSecondaryObject(
            (SDSecondaryObject *)(D_8009B458 + soff),
            (SDSecondaryRecord *)(D_8009B458 + ch * SD_SEQUENCE_CHANNEL_RECORD_SIZE));
        *(s16 *)(D_8009B458 + 0x4C8) =
            (((SDSecondaryObject *)obj)->level_left *
             *(u16 *)(D_8009B458 + 0x514)) >> 7;
        *(s16 *)(D_8009B458 + 0x4CA) =
            (((SDSecondaryObject *)obj)->level_right *
             *(u16 *)(D_8009B458 + 0x516)) >> 7;
        obj[5] = level;
        *(s16 *)(obj + 0x1C) = rec[7];
        *(s16 *)(obj + 0x1A) = -1;
        pitch = SD_CalcPitchBend((SDSecondaryObject *)obj, rec[7]) + obj[6] * 128;
        *(s16 *)(D_8009B458 + 0x4D4) =
            func_80049FB4((s16)pitch >> 7, pitch & 0x7F, tone[4], tone[5]);
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
