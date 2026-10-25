#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_sequence_constants.h"

#include "sound.h"
#include "sound_secondary_object_selection.h"
#include "sd_pitch_from_note.h"
#include "sound_spatialize_object.h"
#include "sound_spatialization.h"
#include "sound_transfer_lifecycle.h"
#include "sound_sequence_channel_events.h"

/* Two secondary-sequencer channel events that sit back to back in the
   image: the reverb parameter (mode or depth) that
   SD_HandleSequenceControlChange applies, and the note-on that the
   sequence parser issues. */

void SD_ApplySequenceReverbParameter(SDSecondaryRecord *entry, int unused)
{
    SpuReverbAttr packet;
    switch (entry->parameter_selector) {
    case SD_SEQUENCE_PARAMETER_REVERB_MODE:
        if (SpuGetReverb() == SPU_ON)
            SpuSetReverb(SPU_OFF);
        if (SpuIsReverbWorkAreaReserved(SPU_CHECK) != 0)
            SpuReserveReverbWorkArea(SPU_OFF);
        packet.mask = SPU_REV_MODE;
        packet.mode = entry->control_value;
        SpuSetReverbModeParam(&packet);
        if (SpuIsReverbWorkAreaReserved(SPU_DIAG) != 0)
            SpuReserveReverbWorkArea(SPU_ON);
        SpuSetReverb(SPU_ON);
        gSD_pSecondaryState->field_0844 = entry->control_value;
        break;
    case SD_SEQUENCE_PARAMETER_REVERB_DEPTH:
        packet.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
        packet.depth.left = entry->control_value << 8;
        packet.depth.right = entry->control_value << 8;
        SpuSetReverbModeParam(&packet);
        SpuSetReverb(SPU_ON);
        gSD_pSecondaryState->field_0845 = entry->control_value;
        break;
    }
}

/*
 * Starts one secondary-driver note: walks the Psy-Q VAB program and tone
 * records for the channel's program, allocates (or steals) a secondary
 * object, fills the driver's SpuVoiceAttr from the tone, applies the pitch
 * and the spatialised volume, keys the voice on and routes its reverb.
 *
 * Matches under uniform gcc_2_8_1_g0 with no register pins, no inline
 * assembly and no symbol aliases. Promoted from src/candidates, where the
 * best state was 24 raw words under the mixed cc_g8_as_g0 profile with an
 * `asm("gSD_pSecondaryState")` alias forcing one root reload. What closed it:
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
 *  - tone->pan goes through the byte local `tr`; the earlier candidate reused
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
 * Carried over from the candidate: unsigned key and tone indices,
 * block-scoped reverb masks, the folded VAB indices, two allocation call
 * sites, the two-step `key`, `vi` as a second name for the voice index after
 * the pitch-bend call, `td` and `nb` as early loads, `level` assigned inside
 * the tone loop (loop.c hoists it), and the declaration order `used`,
 * `program`, `vab`, `rec` that lays out the spill slots.
 *
 * The state is reached through the typed SDSecondaryState members (channel
 * records, objects, transfer pointers, voice_attr and the two output-level
 * words); the candidate's byte view and its `soff` byte offset are not
 * needed for the match.
 */
void SD_KeyOnSecondaryNote(arg0, arg1, velocity)
s32 arg0;
s32 arg1;
u8 velocity;
{
    s32 channel;
    s32 note;
    SDVabProgram *prog;
    s32 used;
    u8 program;
    u8 *vab;
    SDSecondaryRecord *rec;
    s32 ch;
    u32 tidx;
    u32 key;
    SDVabHeader *hdr;
    SDVabTone *tone;
    SDSecondaryObject *obj;
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
    s32 vi;
    u16 adsr1;
    u16 adsr2;
    u8 tr;
    s32 nb;
    u8 td;

    channel = arg0;
    note = arg1;
    hdr = (SDVabHeader *)gSD_pSecondaryState->transfer.vab_header;
    rec = &gSD_pSecondaryState->channels[channel & 0xFF];
    program = rec->program;
    vab = (u8 *)hdr;
    used = 0;
    i = 0;
    if (program != 0) {
        do {
            if (((SDVabProgram *)(vab + SD_VAB_PROGRAM_TABLE_OFFSET))[i].tone_count != 0) {
                used++;
            }
            i++;
        } while (i < program);
    }
    prog = &((SDVabProgram *)(vab + SD_VAB_PROGRAM_TABLE_OFFSET))[program];
    if (prog->tone_count == 0) {
        return;
    }
    tidx = 0;
    key = note;
    key = key & 0xFF;
    ch = channel & 0xFF;
    do {
        level = note & 0x7F;
        tone = &((SDVabTone *)(vab + SD_VAB_TONE_TABLE_OFFSET))
                    [used * SD_VAB_TONES_PER_PROGRAM + (tidx & 0xFFFF)];
        if (key < tone->note_min) {
            goto next;
        }
        if (tone->note_max < key) {
            goto next;
        }
        {
            u32 limit = rec->voice_usage & 0xF0;

            stolen = 0;
            if (limit != 0) {
                if ((rec->voice_usage & 0xF) >= (limit >> 4)) {
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
        vag = tone->vag;
        sizes = (u16 *)(gSD_pSecondaryState->transfer.vab_header
                        + hdr->program_count * 512 + 0x820);
        obj = &gSD_pSecondaryState->objects[idx];
        if (vag > 0) {
            do {
                sum = sum + sizes[i];
                i++;
            } while (i < vag);
        }
        sum = sum + sum * 7;
        voice = &gSD_adwVoiceMasks[idx];
        vm = *voice;
        gSD_pSecondaryState->voice_attr.mask = 0x6019F;
        gSD_pSecondaryState->voice_attr.volmode.left = 0;
        gSD_pSecondaryState->voice_attr.volmode.right = 0;
        nb = (s32)gSD_pSecondaryState->transfer.spu_body_base;
        gSD_pSecondaryState->voice_attr.voice = vm;
        gSD_pSecondaryState->voice_attr.addr = nb + sum;
        adsr1 = tone->adsr1;
        gSD_pSecondaryState->voice_attr.adsr1 = adsr1;
        obj->adsr1 = adsr1;
        adsr2 = tone->adsr2;
        gSD_pSecondaryState->voice_attr.adsr2 = adsr2;
        obj->adsr2 = adsr2;
        if ((tone->adsr1 & 0x80) == 0) {
            amode = 1;
            gSD_pSecondaryState->voice_attr.a_mode = amode;
        } else {
            amode = 5;
            gSD_pSecondaryState->voice_attr.a_mode = amode;
        }
        obj->a_mode = gSD_pSecondaryState->voice_attr.a_mode;
        rec->velocity = velocity;
        obj->center_note = tone->center_note;
        obj->center_fine = tone->center_fine;
        obj->pitch_bend_negative_scale = tone->pitch_bend_min;
        td = tone->pitch_bend_max;
        obj->voice_index = idx;
        obj->program = program;
        obj->tone_index = tidx;
        obj->note = obj->key_note = level;
        obj->channel_index = channel;
        obj->lifecycle_stage = 1;
        obj->pitch_bend_positive_scale = td;
        obj->program_volume = prog->volume;
        obj->program_pan = prog->pan;
        obj->tone_volume = tone->volume;
        tr = tone->pan;
        obj->velocity = velocity;
        obj->reuse_countdown = 0xFFFF;
        obj->tone_pan = tr;
        SD_SpatializeSecondaryObject(
            &gSD_pSecondaryState->objects[idx],
            &gSD_pSecondaryState->channels[ch]);
        gSD_pSecondaryState->voice_attr.volume.left =
            (obj->level_left *
             gSD_pSecondaryState->master_level_left) >> 7;
        gSD_pSecondaryState->voice_attr.volume.right =
            (obj->level_right *
             gSD_pSecondaryState->master_level_right) >> 7;
        obj->key_note = level;
        obj->field_001C = rec->pitch_bend_msb;
        obj->cached_pitch_bend = -1;
        pitch = SD_CalcPitchBend(obj, rec->pitch_bend_msb) + obj->note * 128;
        vi = idx;
        gSD_pSecondaryState->voice_attr.pitch =
            SD_PitchFromNote((s16)pitch >> 7, pitch & 0x7F, tone->center_note,
                         tone->center_fine);
        SpuSetKeyOnWithAttr(&gSD_pSecondaryState->voice_attr);
        if (stolen == 0) {
            if ((rec->voice_usage & 0xF) < 0xF) {
                rec->voice_usage = rec->voice_usage + 1;
            }
        }
        obj->key_held = 1;
        if (rec->reverb_mode == 0) {
            if ((tone->mode & 4) != 0) {
                s32 *m = &gSD_adwVoiceMasks[vi];
                do {
                    SpuSetReverbVoice(SPU_ON, *m);
                } while ((SpuGetReverbVoice() & *m) == 0);
            } else {
                s32 *m = &gSD_adwVoiceMasks[vi];
                do {
                    SpuSetReverbVoice(SPU_OFF, *m);
                } while ((SpuGetReverbVoice() & *m) != 0);
            }
        } else if (rec->reverb_mode != 1) {
            s32 *m = &gSD_adwVoiceMasks[vi];
            do {
                SpuSetReverbVoice(SPU_ON, *m);
            } while ((SpuGetReverbVoice() & *m) == 0);
        } else {
            s32 *m = &gSD_adwVoiceMasks[vi];
            do {
                SpuSetReverbVoice(SPU_OFF, *m);
            } while ((SpuGetReverbVoice() & *m) != 0);
        }
    next:
        tidx++;
    } while ((tidx & 0xFFFF) < prog->tone_count);
}
