/*
 * Starts one secondary-driver note by walking Psy-Q VAB program/tone data,
 * allocating a voice, filling SpuVoiceAttr, applying pitch/spatial volume,
 * keying on, and routing reverb. Current best under gcc_2_8_1_cc_g8_as_g0:
 * 355/355 instructions at exact length, an EMPTY opcode census by encoded
 * fields (bits 31-26, SPECIAL by funct), 344 of 355 aligned on opcode and
 * registers in 4 structural blocks, 14 raw words differing with relocations
 * masked, 5 with the register fields masked as well, and a shift-aware
 * structural distance of 5 (difflib over the register-masked words). The
 * previous state was 355, census 0, 9 structural blocks, 339 aligned, 24
 * raw, 17 register-masked, shift-aware 16.
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
 *    the census (register-masked 34 -> 31, shift-aware 30 -> 27);
 *  - the pitch shift count is the literal 7, not `channel = 7;` reused: the
 *    reuse materialised the 7 into channel's callee-saved register in a
 *    branch delay slot where retail has `li v0,1` (shift-aware 27 -> 25,
 *    register-masked 31 -> 28, four register-only words);
 *  - `channel = tone[3];` written right after the tone[2] store, before the
 *    velocity and 0xFFFF stores (permuter find): the load sits where retail
 *    issues it and the parameter keeps s7 (75 -> 69 raw, 28 -> 21, 25 -> 19);
 *  - `vi = idx;` a second name after the pitch-bend call, used by the four
 *    reverb-mask computations (permuter find): the split live range gives
 *    `tone` s2 and `idx` s1 as retail has them, the allocation the `tone`
 *    pointer lever above had reversed (69 -> 44 raw);
 *  - `key = note; key = key & 0xFF;` and `ch = channel; ch = ch & 0xFF;` as
 *    two statements each, `hdr` read before `rec`, and `sum = sum + sizes[i]`
 *    (permuter finds; the two-step `key` alone is 44 -> 36): 44 -> 40 raw,
 *    21 -> 18 register-masked;
 *  - `obj[2] = program;` written before `obj[4] = tidx;`: retail reloads the
 *    program spill and materialises the 1 before the tidx store (40 -> 38,
 *    18 -> 15, 16 -> 14);
 *  - the loop-entry assignments in the order `ch`, `tidx`, `key`: `tidx`
 *    takes s5, `key` s4 and `ch` s6 as retail has them (38 -> 24 raw; the
 *    register-masked count and the shift-aware distance rise by two each,
 *    from the 55-56 emission order below, and the trade is taken for the
 *    fourteen raw words);
 *  - the voice-mask table entry is read into `tv` (`s32 tv;` declared after
 *    `voice`) BEFORE the 0x6019F store and stored into +0x4C0 where it was:
 *    retail issues the table load above the three SpuVoiceAttr stores
 *    (words 136-142). The stores go through the driver root, a pointer
 *    loaded from memory, so gcc 2.8.1's dependence test cannot separate the
 *    table load from them (`memrefs_conflict_p` returns 1 for an unknown
 *    base against anything), and the -dR dump shows the load with true
 *    dependences on all three; written above them the order is an
 *    anti-dependence, which is what the source has. The `const` route the
 *    workflow names for SD_SetVoiceVolume (`D_80011434_IS_CONST`) does not
 *    reach it here: an INDIRECT_REF is unchanging only when it is
 *    `TREE_READONLY & TREE_STATIC` (expr.c), so `*voice` through a const
 *    pointer is byte-identical to the non-const source, and the direct
 *    `D_80011434[idx]` with the guard is unchanging but changes the
 *    addressing in this profile (no -mgas): one instruction short. 24 -> 14
 *    raw, 17 -> 5 register-masked, 16 -> 5 shift-aware, 9 -> 4 blocks;
 *    `nb` read before the stores as well is identical, `nb` before `tv` is
 *    340 aligned;
 *  - from before: unsigned key/tone indices, uncached driver-root loads,
 *    block-scoped reverb masks, folded VAB indices, two allocation call
 *    sites, one forced root reload, tone[2] read inline at its single use,
 *    `obj[6] = obj[5] = level;` as one chained assignment (both orders tie),
 *    and the key's upper bound tested against the record like the lower.
 *
 * What #5237 (SD_SetVoiceVolume, matched on `D_80011434_IS_CONST` with
 * struct-member stores through a typed state pointer) taught, and what it
 * does here: the const load is free to move across member stores. The same
 * block in this function (words 128-143, where retail hoists the `*voice`
 * load above the 0x6019F store) was rewritten that way in six forms, and
 * every one changes the instruction count (-2 to +1), because the typed
 * pointer folds the root reloads retail repeats. In this byte-view unit
 * the cast stores are what keep the reloads, and the block stays as it is.
 *
 * MEASURED AND DEAD on this base: the const guard with a const `voice`
 * pointer, with const mask pointers as well, or alone (all identical); the
 * direct `D_80011434[idx]` with the guard (-1) and with the four mask reads
 * inline as well (-9); an empty `do { } while (0);` before the spatialize
 * offset or a pin around it, meant to make `idx * 4` recompute as retail
 * does at word 173 (+1 and +2: the note is a scheduling barrier and cse2
 * merges the two products anyway); `(idx * 5) * 8` for the offset
 * (identical). From
 * the base before: tone[3] through a u8 temporary in three
 * spellings (retail loads it into v1; each opens the census with an extra
 * `andi` or `lbu`, and dropping the `channel` assignment is +3);
 * `obj[0xD] = 1` before the tidx store; `program` read into a named byte;
 * the six member-store forms above; decomp-permuter from this state
 * saturates at two outputs, neither at exact length with an empty census.
 * From the earlier base: `idx` declared before `hdr`, first of all locals,
 * or `tone` declared last; a named 0x820 shared by the tone address and the
 * sizes address; `tone` given a birth before the loop; both `channel`
 * reuses removed together (+4 instructions); D_80011434 as `const` with
 * const pointers (nothing) or read inline instead of through `voice` (-1);
 * the amode as a ternary, inline or with the branch flipped; a named -1 for
 * the idx test (register-only, +1 raw); `used = 0; i = 0;` in the other
 * order or at the function top.
 *
 * Residual, 14 words: the velocity spill store and the `used = 0` store in
 * the other order (18-19); `move s5,zero` and `andi s6,s7,0xff` in the
 * other order (55-56, with the parameter's register at 57); `idx * 4`
 * computed once into a3 and reused (130, 133, 176) where retail computes
 * it twice, the second time into the spatialize argument's delay slot at
 * 173 that this source leaves a `nop`; the amode constant in v1 where
 * retail has v0 (157-159); the tone[3] byte in s7 where retail has v1
 * (202, 207). See config/slus_01411/candidates.json.
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
    s32 tv;
    s32 soff;
    s32 vi;
    u16 adsr1;
    u16 adsr2;
    u8 tr;
    s32 nb;
    u8 td;

    channel = arg0;
    hdr = *(u8 **)(D_8009B458 + 0x4A8);
    rec = D_8009B458 + (channel & 0xFF) * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
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
    ch = channel;
    ch = ch & 0xFF;
    tidx = 0;
    key = note;
    key = key & 0xFF;
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
                sum = sum + sizes[i];
                i++;
            } while (i < vag);
        }
        sum <<= 3;
        voice = &D_80011434[idx];
        tv = *voice;
        *(s32 *)(D_8009B458 + 0x4C4) = 0x6019F;
        *(s16 *)(D_8009B458 + 0x4CC) = 0;
        *(s16 *)(D_8009B458 + 0x4CE) = 0;
        nb = *(s32 *)(D_8009B458 + 0x4B8);
        *(s32 *)(D_8009B458 + 0x4C0) = tv;
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
        obj[2] = program;
        obj[4] = tidx;
        obj[6] = obj[5] = level;
        obj[3] = channel;
        obj[0xD] = 1;
        obj[0x10] = td;
        ((SDSecondaryObject *)obj)->field_0008 = prog[1];
        ((SDSecondaryObject *)obj)->field_000A = prog[4];
        ((SDSecondaryObject *)obj)->field_0009 = tone[2];
        channel = tone[3];
        ((SDSecondaryObject *)obj)->field_000E = velocity;
        *(u16 *)(obj + 0x1E) = 0xFFFF;
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
        vi = idx;
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
                s32 *m = &D_80011434[vi];
                do {
                    SpuSetReverbVoice(SPU_ON, *m);
                } while ((SpuGetReverbVoice() & *m) == 0);
            } else {
                s32 *m = &D_80011434[vi];
                do {
                    SpuSetReverbVoice(SPU_OFF, *m);
                } while ((SpuGetReverbVoice() & *m) != 0);
            }
        } else if (rec[0x10] != 1) {
            s32 *m = &D_80011434[vi];
            do {
                SpuSetReverbVoice(SPU_ON, *m);
            } while ((SpuGetReverbVoice() & *m) == 0);
        } else {
            s32 *m = &D_80011434[vi];
            do {
                SpuSetReverbVoice(SPU_OFF, *m);
            } while ((SpuGetReverbVoice() & *m) != 0);
        }
    next:
        tidx++;
    } while ((tidx & 0xFFFF) < prog[0]);
}
