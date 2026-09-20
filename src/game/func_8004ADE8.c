/*
 * Starts one secondary-driver note: walks the Psy-Q VAB program and tone
 * records for the channel's program, allocates (or steals) a secondary
 * object, fills the driver's SpuVoiceAttr from the tone, applies the pitch
 * and the spatialised volume, keys the voice on and routes its reverb.
 *
 * Matches under uniform gcc_2_8_1_g0 with no register pins, no inline
 * assembly and no symbol aliases. Promoted from src/candidates, where the
 * best state was 24 raw words under the mixed cc_g8_as_g0 profile with an
 * `asm("D_8009B458")` alias forcing one root reload. What closed it:
 *
 *  - `amode` is stored inside both arms of the ADSR test. jump2 cross-jumps
 *    the two identical stores, which gives retail's `li v0,5` in the delay
 *    slot and `li v0,1` on the fall-through into one `sw`; the join label is
 *    then a real CSE boundary, so the read of +0x4E4 that follows reloads
 *    the root by itself (no alias), the `idx << 2` of the voice-mask address
 *    is not shared into the byte-store block, and the constants are
 *    block-local pseudos in v0. With the assignments outside the arms jump.c
 *    turns the `if` into a skipped block that CSE follows and all three
 *    effects reverse.
 *  - the voice mask is read into `vm` before the 0x6019F store: the load
 *    sits above the store group as retail has it.
 *  - tone[3] goes through the byte local `tr`; the earlier candidate reused
 *    `channel` for it to keep `channel` in s7. The nested `do { } while (0)`
 *    around the plain SD_SelectSecondaryObject call replaces that: the
 *    argument's reference is counted at loop depth 3, which keeps `channel`
 *    above `prog` in global-alloc priority (s7 over fp) with no code.
 *  - `ch` is a single `& 0xFF` after `tidx` and `key`: key takes s4, tidx s5
 *    and ch s6 both by emission order and by allocation priority.
 *  - `sum = sum + sum * 7` is what combine folds back into `sll a2,a2,3`;
 *    flow has counted the extra references first, so `sum` outranks `i` and
 *    takes a2, leaving a3 for `i` in both loops.
 *  - `note` is copied from its parameter like `channel`. With both copies
 *    merged away, the byte parameter's copy from $a2 is the first insn
 *    after the deleted notes and sched1 pins it at the block head instead of
 *    treating it as a birthing insn that sinks; that is retail's
 *    `sb a2` before `sw zero` for `used = 0`.
 *  - K&R definition with `u8 velocity` behind an int prototype: the unit's
 *    own header keeps the caller's argument setup unchanged.
 *
 * Carried over from the candidate: unsigned key and tone indices, uncached
 * root loads through the byte view of the state, block-scoped reverb masks,
 * the folded VAB indices, two allocation call sites, the two-step `key`,
 * `vi` as a second name for the voice index after the pitch-bend call,
 * `soff` for the spatialize argument, `td` and `nb` as early loads, `level`
 * assigned inside the tone loop (loop.c hoists it), and the declaration
 * order `used`, `program`, `vab`, `rec` that lays out the spill slots.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_sequence_constants.h"

/* The driver state is walked as bytes with the retail offsets: the typed
   view folds the root reloads that retail repeats. */
#define SDSECONDARYSTATE_AS_BYTES
#include "sound.h"
#include "sound_secondary_object_selection.h"
#include "func_80049FB4.h"
#include "sound_spatialize_object.h"
#include "sound_spatialization.h"
#include "func_8004ADE8.h"

void func_8004ADE8(arg0, arg1, velocity)
s32 arg0;
s32 arg1;
u8 velocity;
{
    s32 channel;
    s32 note;
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
    s32 vm;
    s32 soff;
    s32 vi;
    u16 adsr1;
    u16 adsr2;
    u8 tr;
    s32 nb;
    u8 td;

    channel = arg0;
    note = arg1;
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
    tidx = 0;
    key = note;
    key = key & 0xFF;
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
        do {
            do {
                idx = SD_SelectSecondaryObject(channel & 0xFF, key);
            } while (0);
        } while (0);
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
        sum = sum + sum * 7;
        voice = &D_80011434[idx];
        vm = *voice;
        *(s32 *)(D_8009B458 + 0x4C4) = 0x6019F;
        *(s16 *)(D_8009B458 + 0x4CC) = 0;
        *(s16 *)(D_8009B458 + 0x4CE) = 0;
        nb = *(s32 *)(D_8009B458 + 0x4B8);
        *(s32 *)(D_8009B458 + 0x4C0) = vm;
        *(s32 *)(D_8009B458 + 0x4DC) = nb + sum;
        adsr1 = *(u16 *)(tone + 0x10);
        *(s16 *)(D_8009B458 + 0x4FA) = adsr1;
        *(u16 *)(obj + 0x20) = adsr1;
        adsr2 = *(u16 *)(tone + 0x12);
        *(s16 *)(D_8009B458 + 0x4FC) = adsr2;
        *(u16 *)(obj + 0x22) = adsr2;
        if ((*(u16 *)(tone + 0x10) & 0x80) == 0) {
            amode = 1;
            *(s32 *)(D_8009B458 + 0x4E4) = amode;
        } else {
            amode = 5;
            *(s32 *)(D_8009B458 + 0x4E4) = amode;
        }
        *(u16 *)(obj + 0x24) = *(u16 *)(D_8009B458 + 0x4E4);
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
        tr = tone[3];
        ((SDSecondaryObject *)obj)->field_000E = velocity;
        *(u16 *)(obj + 0x1E) = 0xFFFF;
        ((SDSecondaryObject *)obj)->field_000B = tr;
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
