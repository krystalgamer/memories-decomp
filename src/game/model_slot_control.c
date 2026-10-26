/*
 * The per-slot model runtime: the per-frame slot animation tick
 * (Model_UpdateSlotAnimation), the per-slot control step (Model_UpdateSlotControl), slot reset and
 * load (Model_ResetSlot, Model_LoadSlot), the staged slot loader from
 * Model_LoadMonsterMerge through Model_LoadSpecialBattlePackageStage, and
 * Model_ControlSlotAnimation.
 *
 * Every feature switch below selects a header view for the whole unit, so it
 * comes before the first include. MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY is the
 * control step's view; the other three are the staged loader's.
 */
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#define D_8009B118_IS_POINTER_IN_DATA
#define DUEL_PACKAGE_STAGE_RAW_ARENAS

#include "../types.h"
#include "model_slot_part_animation.h"
#include "model_scene_controllers.h"
#include "model.h"
#include "model_get_frame_step.h"
#include "model_slot_state_updates.h"
#include "model_effect_state.h"
#include "color_constants.h"
#include "model_link_slot_animation_rows.h"
#include "model_apply_texture_tint.h"
#include "model_word_memory.h"
#include "model_init_light_triplet.h"
#include "model_slot_row_tables.h"
#include "model_slot_support.h"
#include "model_slot_updates.h"
#include "model_slot_control.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/rand.h"
#include "model_graphics_state.h"
#include "model_handler_state.h"
#include "model_transfer_flags.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "model_load_words.h"
#include "duel_card.h"
#include "sound.h"
#include "model_record_tables.h"
#include "file_names.h"
#include "file_transfer.h"
#include "file_package_staging.h"
#include "file_transfer_state.h"
#include "file_transfer_steps.h"
#include "../psyq/libetc.h"
#include "../psyq/stdio.h"
#include "sound_voice_data.h"
#include "script_state.h"
#include "model_load_step.h"
#include "duel_load_package_stage.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"

/* Per-frame animation tick for a model slot. When the slot is active and
   has an animation loaded it takes the current animation's length and this
   tick's frame advance, lets the animation-6 idle/loop cases hand off through
   Model_SetSlotPlaybackDirection, runs the sound trigger for the two duelist slots, advances
   or rewinds the frame counter (state 0x3E plays forward and clamps, other
   states play backward and wrap), and when the animation has run out with
   no hold pending it switches to the queued animation through Model_SeekSlotAnimation
   and its follow-ups. Finally, unless a slow 0x23-state animation is on an
   off tick, it updates every attached part through Model_SeekSlotPartAnimation at the
   new frame. */
void Model_UpdateSlotAnimation(s32 index) {
    ModelSlot *m;
    s32 anim;
    s32 length;
    s32 speed;
    s32 i;
    s32 frame;
    s32 lim;

    m = &gModel_aSlots[index];
    if (m->is_active == 0) {
        return;
    }
    if (m->animation_commands != 0) {
        anim = m->current_animation_row;
        length = m->animation_duration_rows[anim].max << 4;
        speed = m->playback_speed * Model_GetFrameStep();
        if (anim == 6) {
            lim = length - speed;
            if ((m->playback_state == 0x3E && m->animation_position >= lim) ||
                (m->playback_state == 0x3C && m->animation_position < 2)) {
                Model_SetSlotPlaybackDirection(index, 0);
            }
        }
        if (index < 2) {
            Model_PlaySlotAnimationSounds(index);
        }
        if (m->playback_state != 0x23) {
            if (m->playback_state == 0x3E) {
                s32 v;

                if (m->animation_position >= length) {
                    m->animation_position = 0;
                }
                v = m->animation_position + speed;
                if (v >= length) {
                    m->animation_position = length;
                } else {
                    m->animation_position = v;
                }
            } else {
                s32 w;

                if (m->animation_position == 0) {
                    m->animation_position = length;
                }
                w = m->animation_position - speed;
                if (w <= 0) {
                    m->animation_position = 0;
                } else {
                    m->animation_position = w;
                }
            }
        }
        if (m->part_command_reset_pending == 0) {
            if (m->playback_state == 0x3E ? m->animation_position >= length : m->animation_position == 0) {
                if (m->tint[7] == 6) {
                    m->queued_animation_row = 2;
                }
                if (m->queued_animation_row != m->queued_animation_row_latch) {
                    Model_SeekSlotAnimation(index, m->queued_animation_row, 0);
                    m->queued_animation_row_latch = m->queued_animation_row;
                    if (m->active_animation_command_row != 0) {
                        Model_ControlSlotAnimation(index, 0, 0);
                    }
                    if (m->tint[7] == 6) {
                        Model_QueueEffectEndpointPair(-1, index + 2, m->animation_duration_rows[2].max);
                        m->tint[7] = 0;
                        m->queued_animation_row = 1;
                    }
                }
            }
        }
    }
    if (m->playback_state != 0x23) {
        if (m->playback_speed == 0x10) {
            if (Model_GetFrameStep() == 2) {
                return;
            }
        }
    }
    frame = m->animation_position;
    if (m->animation_sequence_count != 0) {
        i = 0;
        do {
            Model_SeekSlotPartAnimation(m, i, m->current_animation_row, frame);
            i++;
        } while (i < m->animation_sequence_count);
    }
}

/*
 * All 1864 bytes and the six-entry jump table match under the existing
 * uniform gcc_2_8_1_g8_split profile. The module callbacks consume the two
 * stored contexts; the secondary callback's result drives the state switch.
 *
 * Keep persistent callback decisions separate from transient reaction,
 * fallback and sound choices. Signed record-address cursors preserve the
 * parent/subrecord calculations without claiming adjacent subrecords form
 * a compact array. The initial base capture, reaction record capture and
 * final card-id capture keep the measured allocation and load-delay order.
 *
 * The first comparison retains a separate stats base and promotes its own
 * halfword before the opposing halfword. Updating the parent base in place
 * instead lets the first scheduler move that adjustment too early.
 * No hard-register bindings or instruction assembly are used.
 */

#define MODEL_SLOT_CF8_TAIL_VIEW(address) ((ModelSlotCF8TailView *)(address))

/* The state trace printed on the debug font. The Japanese build carries none
   of these prints, so a regional build defines it away. */
#ifndef MODEL_CONTROL_TRACE
#define MODEL_CONTROL_TRACE(message) FntPrint(message)
#endif

void Model_UpdateSlotControl(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];
    ModelSlot *other;
    ModelControlHandler handler;
    ModelControlHandler primary_handler;
    u8 *primary = (u8 *)slot->primary_module_data_address;
    u8 *secondary = (u8 *)slot->variant_module_data_address;
    u16 *own_stats;
    u16 *other_stats;
    u8 *stats_base;
    s32 command;
    s32 action;
    s32 state;
    s32 changed;
    s32 amount;
    s32 frame;
    s32 part;
    s32 primary_state;

    if (slot->is_active == 0) {
        return;
    }
    primary_state = ((ModelControlCommandView *)slot)->commands[2];
    gModel_bActiveSlotIndex = index;
    if (primary_state >= 0) {
        if (index != 0) {
            primary_handler = (ModelControlHandler)(gModel_pSlot1PrimaryModule + 4);
        } else {
            primary_handler = (ModelControlHandler)(gModel_pSlot0PrimaryModule + 4);
        }
        Model_SetHandlerCallbackState(1);
        primary_handler(primary, -1);
        Model_SetHandlerCallbackState(0);
    }
    if (slot->animation_control_state == 6) {
        if (slot->current_animation_row == slot->guardian_star_selection + 3
            || slot->model_id == MODEL_SPECIAL_BATTLE_ID) {
            slot->animation_control_state = 7;
        } else {
            slot->animation_control_state = 2;
        }
    }
    state = slot->animation_control_state;
    if (state == 2) {
        return;
    }
    if (((ModelControlCommandView *)slot)->commands[slot->guardian_star_selection] < 0) {
        goto no_handler;
    }
    if (index != 0) {
        handler = (ModelControlHandler)((u8 *)gModel_Slot1VariantModuleAddress + 4);
    } else {
        handler = (ModelControlHandler)((u8 *)gModel_Slot0VariantModuleAddress + 4);
    }
    if (slot->animation_control_state == 7) {
        Model_SetHandlerCallbackState(1);
        {
            s32 request = ((ModelControlCommandView *)slot)->commands[slot->guardian_star_selection];
            handler(secondary, request % 1000);
        }
        Model_SetHandlerCallbackState(0);
        slot->animation_control_state = 8;
        slot->animation_sound_position = 0;
    }
    if (slot->current_animation_row != slot->guardian_star_selection + 3
        && slot->model_id != MODEL_SPECIAL_BATTLE_ID) {
        s32 eligible = slot->animation_control_state;
        if (eligible != 0 && eligible != 3 && eligible != 1 && eligible != 4 && eligible != 5) {
            return;
        }
    }
    {
        s32 selected;
        s32 base;
        s32 stat_base;
        s32 own_offset;
        s32 other_offset;
        s32 own_record;
        s32 other_record;
        other_offset = (index ^ 1) * MODEL_SLOT_SIZE;
        do { base = (s32)gModel_aSlots; } while (0);
        other = (ModelSlot *)(other_offset + base);
        own_offset = index * MODEL_SLOT_SIZE;
        stat_base = base + (u32)&((ModelSlot *)0)->field_CF8;
        own_record = own_offset + stat_base;
        other_record = other_offset + stat_base;
        if (other->defense_position != 0) {
            {
                u16 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
                u16 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_02;
                selected = own_value > other_value ? 6 : 8;
            }
        } else {
            u16 own_value =
                MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
            u16 other_value =
                MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_00;
            selected = own_value > other_value ? 6 : 5;
            if (own_value == other_value) {
                selected |= 0x80;
                if (index != 0) {
                    selected = 6;
                }
            }
        }
        action = selected;
    }
    Model_SetHandlerCallbackState(1);
    slot->animation_control_state = handler(secondary, -1);
    Model_SetHandlerCallbackState(0);
    switch (slot->animation_control_state) {
    case 0:
        MODEL_CONTROL_TRACE(gModel_szContinueTrace);
        return;
    case 5:
        MODEL_CONTROL_TRACE(gModel_szBackTrace);
        if (other->active_animation_command_row == 5 && other->animation_position >= 0x60) {
            Model_ControlSlotAnimation(index ^ 1, 1, 1);
        }
        return;
    case 3:
        MODEL_CONTROL_TRACE(gModel_szStoneTrace);
        if (action == 6) {
            other->stone_texture_state = 0x80;
            Model_ControlSlotAnimation(index ^ 1, 6, 1);
            return;
        }
        if (action & 0x80) {
            other->stone_texture_state = 0xFF;
            Model_ControlSlotAnimation(index ^ 1, 5, 1);
            return;
        }
    case 1:
    case 4: {
        s32 reaction;
        s32 offset;
        s32 own_record;
        s32 other_record;
        amount = Model_GetFrameStep();
        offset = index * MODEL_SLOT_SIZE;
        do { own_record = offset + (s32)&gModel_Slot0MetadataView; } while (0);
        offset = (index ^ 1) * MODEL_SLOT_SIZE;
        other_record = offset + (s32)&gModel_Slot0MetadataView;
        amount = slot->playback_speed * (amount + 1);
        if ((((u8 *)&gModel_Slot0MetadataView) + offset)[MODEL_SLOT_CF8_DFF_OFFSET] != 0) {
            {
                u32 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
                u32 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_02;
                reaction = other_value < own_value ? 6 : 8;
            }
        } else {
            u32 own_value =
                MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
            u32 other_value =
                MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_00;
            reaction = other_value < own_value ? 6 : 5;
            if (own_value == other_value) {
                reaction |= 0x80;
                if (index != 0) {
                    reaction = 6;
                }
            }
        }
        command = reaction & 0x7F;
        changed = 0;
        if (slot->animation_control_state == 1) {
            MODEL_CONTROL_TRACE(gModel_szHitTrace);
        }
        if (slot->animation_control_state == 4) {
            MODEL_CONTROL_TRACE(gModel_szOuchTrace);
            if (slot->animation_control_state == 4 && command == 6 && other->active_animation_command_row != 6) {
                command = 5;
            }
        }
        if (other->active_animation_command_row == command) {
            if (other->playback_state != 0x23 && other->animation_position >= 0xA0 &&
                ((rand() >> 8) & 3) != 0) {
                other->animation_position -= amount;
                frame = other->animation_position;
                for (part = 0; part < other->animation_sequence_count; part++) {
                    Model_SeekSlotPartAnimation(other, part, other->current_animation_row, frame);
                }
                if ((rand() >> 8) & 1) {
                    changed++;
                }
            }
        } else {
            Model_ControlSlotAnimation(index ^ 1, command, 1);
            changed++;
        }
        if (gModel_bSceneMode == 0xF && changed != 0 && command == 8) {
            u32 card_id;
            s32 sound_id;
            do { card_id = other->model_id; } while (0);
            sound_id = ((gDuel_adwCardStats[card_id] >> 26) & 0x1F) == 1 ? 3 : 4;
            SD_SEPlay(sound_id, 0xFF, 0);
        }
        return;
    }
    case 2:
        MODEL_CONTROL_TRACE(gModel_szTermTrace);
        return;
    default:
        MODEL_CONTROL_TRACE(gModel_szWhyTrace);
        slot->animation_control_state = 2;
        return;
    }

no_handler:
    {
        s32 fallback_action;
        s32 offset;
        s32 own_record;
        s32 other_record;
        if (slot->current_animation_row == slot->guardian_star_selection + 3) {
            s32 current_frame = slot->animation_position >> 4;
            s32 midpoint = slot->animation_duration_rows[slot->current_animation_row].max >> 1;
            if (current_frame >= midpoint) {
                offset = index * MODEL_SLOT_SIZE;
                own_record = offset + (s32)&gModel_Slot0MetadataView;
                offset = (index ^ 1) * MODEL_SLOT_SIZE;
                other_record = offset + (s32)&gModel_Slot0MetadataView;
                if ((((u8 *)&gModel_Slot0MetadataView) +
                     offset)[MODEL_SLOT_CF8_DFF_OFFSET] != 0) {
                    {
                        u32 own_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                                prefix.values.field_00;
                        u32 other_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                                prefix.values.field_02;
                        fallback_action = other_value < own_value ? 6 : 8;
                    }
                } else {
                    u32 own_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                            prefix.values.field_00;
                    u32 other_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                            prefix.values.field_00;
                    fallback_action = other_value < own_value ? 6 : 5;
                    if (own_value == other_value) {
                        fallback_action |= 0x80;
                        if (index != 0) {
                            fallback_action = 6;
                        }
                    }
                }
                Model_ControlSlotAnimation(index ^ 1, fallback_action & 0x7F, 1);
                slot->animation_control_state = 2;
            }
        } else if (state == 8) {
            slot->animation_control_state = 2;
        }
        if (slot->animation_control_state == 7) {
            slot->animation_control_state = 8;
        }
    }
}

/* One model slot's setup: the reset that gives it its defaults (0x8005611C)
   and the per-frame duel-side layout pass that reads them (0x80056250). The
   two are contiguous -- 0x8005611C is 0x134 bytes and ends exactly at
   0x80056250 -- and are bounded above by Model_LoadMonsterMerge, which is
   assembly again.

   They are one initializer and its consumer. The reset writes the mode byte
   at +0xE16 as 0x3E, the pair at +0xE0C/+0xE0D as 7 and 8, the colour scale
   at +0xE0A as COLOR_FIXED_ONE and the busy byte at +0xE1F as 0; the layout
   pass switches on that same +0xE16 (0x3E is one of its three cases), reads
   +0xE0C, +0xE0D and +0xE0A back, and sets +0xE1F to 1. */

void Model_ResetSlot(s32 arg0)
{
    ModelSlot *p;
    u8 *q;
    s32 i;
    s32 n;

    p = &gModel_aSlots[arg0];
    Model_FillWords((s32 *)p, 0, 0x388);

    p->ambient_rgb_scale[2] = MODEL_FIXED_HALF;
    p->ambient_rgb_scale[1] = MODEL_FIXED_HALF;
    p->ambient_rgb_scale[0] = MODEL_FIXED_HALF;
    p->render_scale.field_08 = MODEL_FIXED_ONE;
    p->render_scale.field_04 = MODEL_FIXED_ONE;
    p->render_scale.field_00 = MODEL_FIXED_ONE;
    p->tint[2] = 0x80;
    p->tint[1] = 0x80;
    p->tint[0] = 0x80;
    p->texture_tint_control = 7;
    p->playback_speed = 8;
    p->load_step = 0xFF;
    p->tint[3] = 0;
    *(s16 *)&p->texture_tint_scale = COLOR_FIXED_ONE;
    p->skip_sound_setup = 0;
    p->guardian_star_selection = arg0;
    p->defense_position = 0;
    p->field_DFC = 0xFFFF;
    p->field_DFA = 0xFFFF;

    n = 1;
    i = 3;
    q = (u8 *)p + i;
    for (; i >= 0; q--, i--) {
        q[0xBF4] = n;
    }

    for (i = 0; i < 0x40; i++) {
        *(s32 *)&p->sound_entries[i] &= 0x8000FFFF;
        p->sound_entries[i].sound_id = 0;
        p->sound_entries[i].animation_row = 0;
    }

    p->field_CF8.field_10 = -1;
    p->field_CF8.field_14 = -1;
    p->field_CF8.field_18 = -1;
    p->playback_state = 0x3E;
    p->is_active = 0;
    Model_InitLightTriplet(arg0);
}

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * Model_SetSlotPartSpeeds before Model_ApplyTextureTint draws it. */
void Model_LoadSlot(s32 arg0, u8 *arg1, s32 arg2, s32 arg3) {
    ModelSlot *p;
    ModelSlot *q;
    ModelSlotPart *G32 *c;
    ModelSlot *e;
    ModelSlot *r;
    s32 i;
    s32 j;
    s32 sum;
    s32 k;
    s32 v;
    s32 m;
    s32 n;
    u8 *b;
    u8 *b2;
    s32 w;

    Model_InitSlot(arg0, arg1, arg2, arg3);
    if (arg1 == (u8 *)0) {
        return;
    }
    Model_BuildSlotAnimationTables(arg0);
    Model_LinkSlotAnimationRows(arg0);
    p = &gModel_aSlots[arg0];
    sum = 0;
    if (arg0 < 2) {
        i = sum;
        if (sum < p->animation_sequence_count) {
            m = 0xFFFF;
            n = p->animation_sequence_count;
            do {
                if (p->animation_command_indices[1][i] != m) {
                    k = i / 8;
                    if ((p->extended_animation_part_mask[k] >> (i - k * 8)) & 1) {
                        sum += 0x14;
                    } else {
                        sum += 0xC;
                    }
                }
                i++;
            } while (i < n);
        }
    }
    v = (p->lit_color_cache_address = (s32)p->animation_snapshot_buffer + sum);
    p->lit_color_cache_end_address = v + p->lit_color_cache_entry_count * sizeof(u32);
    Model_SetSlotRootTransform(p, 0, 0, 0, arg0 == 1 ? 0x800 : 0);

    q = &gModel_aSlots[arg0];
    c = q->animation_sequences;
    for (j = 0; j < q->animation_sequence_count; j++) {
        (*c)->ii = 0xFFFF;
        (*c)->ti = (*c)->start;
        (*c)->aframe = 0xFFFF;
        (*c)->sid = (*c)->start_sid;
        (*c)->rframe = 0;
        (*c)->speed = 0x10;
        c++;
    }

    b = (u8 *)gModel_aSlots;
    e = (ModelSlot *)(arg0 * MODEL_SLOT_SIZE + b);
    e->is_active = 1;
    Model_DrawSlotUnitsOnly(arg0);
    v = e->playback_speed * 2;
    w = v;
    switch (e->playback_state) {
    case 0x23:
        w = 0;
        Model_SetSlotPartSpeeds(arg0, w);
        break;
    case 0x3E:
        w = v;
        Model_SetSlotPartSpeeds(arg0, w);
        break;
    case 0x3C:
        w = -w;
        Model_SetSlotPartSpeeds(arg0, w);
        break;
    }
    if (arg0 < 2) {
        b2 = (u8 *)gModel_aSlots;
        r = (ModelSlot *)(arg0 * MODEL_SLOT_SIZE + b2);
        Model_ApplyTextureTint(arg0, r->texture_tint_control, r->texture_tint_scale);
    }
}

/* Starts the asynchronous transfer that fills one model slot with a monster
 * merge record, and records the slot's display properties while the request is
 * in flight.
 *
 * The slot argument carries a flag in bit 0x80: when it is clear the sound
 * driver's voices are keyed off first, and the flag itself is stored at
 * +0xE1D.  A negative model id means "reuse the id already in the slot" and a
 * non-negative one is written back to +0xDF8, exactly as
 * Model_SetSlotProperties does; the remaining properties follow the same
 * "negative means leave alone" convention it uses for +0xDFA to +0xDFF.
 *
 * Duel slots (0 and 1) load out of the model MRG. Each compacted model entry
 * occupies MODEL_MRG_SECTOR_COUNT sectors, so the three id ranges that have no
 * record - MODEL_MRG_FIRST_GAP_START through the byte before
 * MODEL_MRG_FIRST_GAP_END, MODEL_MRG_SECOND_GAP_START through the byte before
 * MODEL_MRG_SECOND_GAP_END, and MODEL_MRG_SINGLE_GAP_ID - are rejected and
 * every id above such a gap is biased down by MODEL_MRG_GAP_SIZE.
 * MODEL_SPECIAL_BATTLE_ID is the one special case and comes from its own
 * file. Any other slot loads MODEL_AUX_SECTOR_COUNT sectors selected
 * through the MODEL_AUX_LOOKUP_RECORD_SIZE-byte table at gModel_abBackgroundRecords.
 *
 * Returns zero once a transfer has been requested and one when the model id
 * has no record to request. */
s32 Model_LoadMonsterMerge(s32 slot, s32 model, s32 p2, s32 p3, s32 p4,
                           s32 p5, s32 arg6)
{
    FileTransferDescriptor *transfer;
    s32 flags;
    s32 result;

    result = 0;
    flags = slot & 0x80;
    slot &= 0x7F;
    if (model < 0) {
        model = gModel_aSlots[slot].model_id;
    } else {
        gModel_aSlots[slot].model_id = model;
    }
    if (flags == 0) {
        SD_KeyOffVoiceSlots();
    }
    File_WaitForTransfers();
    Model_InitSlot(slot, 0, 0, arg6);
    gModel_aSlots[slot].skip_sound_setup = flags;
    if (slot < 2) {
        if (model == MODEL_SPECIAL_BATTLE_ID) {
            transfer = File_TryRequestAsyncTransfer(
                1, gModel_szSuMrgPath, MODEL_SPECIAL_BATTLE_FILE_START_SECTOR,
                MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT,
                Model_LoadSpecialBattlePackageStage, 0, 0
            );
            gFile_dwTransferState_abs = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].load_step = 0;
            return 0;
        }
        if (model < 0 || model >= MODEL_MRG_ID_END
            || (model >= MODEL_MRG_FIRST_GAP_START
                && model < MODEL_MRG_FIRST_GAP_END)
            || (model >= MODEL_MRG_SECOND_GAP_START
                && model < MODEL_MRG_SECOND_GAP_END)
            || model == MODEL_MRG_SINGLE_GAP_ID) {
            result++;
        } else {
            if (model >= MODEL_MRG_LAST_ID) {
                model--;
            }
            if (model >= MODEL_MRG_SECOND_GAP_END) {
                model -= MODEL_MRG_GAP_SIZE;
            }
            if (model >= MODEL_MRG_FIRST_GAP_END) {
                model -= MODEL_MRG_GAP_SIZE;
            }
            transfer = File_TryRequestAsyncTransfer(
                2, gFile_szModelMrgPath, model * MODEL_MRG_SECTOR_COUNT,
                MODEL_MRG_SECTOR_COUNT, Model_LoadMonsterMergeStage,
                0, 0
            );
            if (p2 >= 0) {
                gModel_aSlots[slot].field_DFA = p2;
            }
            if (p3 >= 0) {
                gModel_aSlots[slot].field_DFC = p3;
            }
            if (p4 >= 0) {
                gModel_aSlots[slot].guardian_star_selection = p4 != 0;
            }
            if (p5 >= 0) {
                gModel_aSlots[slot].defense_position = p5 != 0;
            }
            transfer->callback_data = (void *)slot;
            transfer->position = gModel_aSlots[slot].guardian_star_selection;
            gFile_dwTransferState_abs = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].load_step = 0;
        }
    } else {
        if (model >= 0 && model < 7) {
            model = gModel_abBackgroundRecords[
                model * MODEL_AUX_LOOKUP_RECORD_SIZE
                + MODEL_AUX_LOOKUP_VALUE_OFFSET];
            if (model >= 0 && model < 7) {
                goto found;
            }
        }
        result++;
        goto done;
found:
        transfer = File_TryRequestAsyncTransfer(
            1, gModel_szSuMrgPath,
            model * MODEL_AUX_SECTOR_COUNT + MODEL_AUX_FILE_START_SECTOR,
            MODEL_AUX_SECTOR_COUNT, Model_LoadAuxiliaryPackageStage, 0, 0
        );
        gFile_dwTransferState_abs = transfer->status_flags
            | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
        gModel_aSlots[slot].load_step = 0;
    }
done:
    return result;
}

static inline s16 load_phase(ModelSlot *slot)
{
    return slot->load_step;
}

void Model_AdvanceSlotLoad(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];
#ifndef VERSION_JAPAN
    s32 started = VSync(1);
#endif
    s32 state = slot->load_step;
    s32 work;
    s32 amount;
    s32 offset;

    if (state == 0 || state == 255) {
        return;
    }
    switch (load_phase(slot)) {
    case 1: {
        u8 *payload = (u8 *)gModel_AuxiliaryPayloadAddress;
        s32 size = 0xC000;
        switch (index) {
        case 0:
            payload = gRuntime_pSharedWorkBase;
            break;
        case 1:
            payload = gModel_pSlot1PayloadBase;
            break;
        }
        if (*(s32 *)payload != 0) {
            size = *(s32 *)payload;
        }
        Model_InitSlot(index, payload, size, -1);
        break;
    }
    case 2:
        Model_BuildSlotAnimationTables(index);
        break;
    case 3:
        Model_LinkSlotAnimationRows(index);
        break;
    case 4: {
        ModelSlot *p = &gModel_aSlots[index];
        s32 sum = 0;
        s32 i, count, byte, missing;
        if (index < 2) {
            i = sum;
            if (sum < p->animation_sequence_count) {
                missing = 0xFFFF;
                count = p->animation_sequence_count;
                do {
                    if (p->animation_command_indices[1][i] != missing) {
                        byte = i / 8;
                        if ((p->extended_animation_part_mask[byte] >> (i - byte * 8)) & 1) {
                            sum += 0x14;
                        } else {
                            sum += 0xC;
                        }
                    }
                    i++;
                } while (i < count);
            }
        }
        amount = (p->lit_color_cache_address = (s32)p->animation_snapshot_buffer + sum);
        p->lit_color_cache_end_address = amount + p->lit_color_cache_entry_count * 4;
        Model_SetSlotRootTransform(p, 0, 0, 0, index == 1 ? 0x800 : 0);
        break;
    }
    case 5: {
        ModelSlot *p = &gModel_aSlots[index];
        ModelSlotPart *G32 *part = p->animation_sequences;
        s32 i;
        for (i = 0; i < p->animation_sequence_count; i++) {
            (*part)->ii = 0xFFFF;
            (*part)->ti = (*part)->start;
            (*part)->aframe = 0xFFFF;
            (*part)->sid = (*part)->start_sid;
            (*part)->rframe = 0;
            (*part)->speed = 0x10;
            part++;
        }
        break;
    }
    case 6:
        slot->is_active = 1;
        Model_DrawSlotUnitsOnly(index);
        slot->is_active = 0;
        break;
    case 7: {
        ModelSlot *p = &gModel_aSlots[index];
        amount = p->playback_speed * 2;
        offset = amount;
        switch (p->playback_state) {
        case 0x23:
            offset = 0;
            Model_SetSlotPartSpeeds(index, offset);
            break;
        case 0x3E:
            offset = amount;
            Model_SetSlotPartSpeeds(index, offset);
            break;
        case 0x3C:
            offset = -offset;
            Model_SetSlotPartSpeeds(index, offset);
            break;
        }
        if (index >= 2) {
            slot->is_active = 1;
        }
        break;
    }
    case 8:
        Model_ApplyTextureTint(index, slot->texture_tint_control, slot->texture_tint_scale);
        break;
    case 9: {
        ModelSlot *p = &gModel_aSlots[index];
        u8 *context;
        ModelControlHandler handler;
        s32 selected;
        s32 position;
        s32 i;
        work = ((ModelControlCommandView *)p)->commands[2];
        context = (u8 *)p->primary_module_data_address;
        gModel_bActiveSlotIndex = index;
        if (work >= 0) {
            if (index != 0) {
                handler = (ModelControlHandler)(gModel_pSlot1PrimaryModule + 4);
            } else {
                handler = (ModelControlHandler)(gModel_pSlot0PrimaryModule + 4);
            }
            Model_SetHandlerCallbackState(1);
            handler(context, ((ModelControlCommandView *)p)->commands[2] % 1000);
            Model_SetHandlerCallbackState(0);
        }
        selected = p->current_animation_row;
        p->animation_control_state = 6;
        position = p->animation_position;
        for (i = 0; i < p->animation_sequence_count; i++) {
            s32 current = p->current_animation_row;
            if (selected != 0) {
                p->animation_sequences[i]->sid = selected;
                current = selected;
            }
            Model_SeekSlotPartAnimation(p, i, current, position);
        }
        if (selected != 0) {
            p->current_animation_row = selected;
        }
        break;
    }
    case 10:
        if (slot->skip_sound_setup == 0) {
            SD_LoadSequenceBankPair(
                index, (u32 *)(D_801A8000 + (index << 11))
            );
        }
        break;
    case 11:
        if (slot->skip_sound_setup == 0) {
            slot->animation_sound_enabled = 1;
        }
        slot->is_active = 1;
        break;
    }
#ifndef VERSION_JAPAN
    work = VSync(1);
    printf(gModel_szLoadModelTraceFormat, slot->load_step, work - started);
#endif
    {
        s32 next;
        if (slot->is_active != 0) {
            next = 255;
        } else {
            next = slot->load_step + 1;
        }
        slot->load_step = next;
    }
}

/*
 * Contiguous model file-transfer phase callbacks. The first handles MRG
 * transfer phases for the two duel slots. The entry slot and
 * alternate-palette selector are captured before dispatch; the finish stage
 * reloads the slot overrides after the copy and optional state callback.
 *
 * All 1,992 text bytes and the 17-entry table match the uniform G8 profile.
 * Separate cleared/reloaded flag captures retain the upload-stage ordering.
 * The direct image argument and post-call source cursor share the same
 * staging buffer without extending the cursor's lifetime across the upload.
 */
#ifndef MODEL_TEXTURE_STAGE3_RECT_X
#define MODEL_TEXTURE_STAGE3_RECT_X(index) ((index) << 8)
#endif
#ifndef MODEL_TEXTURE_STAGE3_RECT_Y
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) 0xF8
#endif
#ifndef MODEL_TEXTURE_PALETTE_RECT_X
#define MODEL_TEXTURE_PALETTE_RECT_X 0x200
#endif
#ifndef MODEL_TEXTURE_TAIL_ROW_Y
#define MODEL_TEXTURE_TAIL_ROW_Y 0xF0
#endif

void Model_LoadMonsterMergeStage(FileTransferDescriptor *d, s32 stage)
{
    RECT rect0;
    RECT rect1;
    s32 index;
    s32 alternate;
    ModelSlot *slot;

    index = (s32)d->callback_data;
    alternate = d->position;
    slot = &gModel_aSlots[index];
    switch (stage) {
    case 0: {
        u8 *destination;
        if (index == 0) {
            destination = gRuntime_pSharedWorkBase;
        } else {
            destination = gModel_pSlot1PayloadBase;
        }
        d->sector_destination = d->alternate_sector_destination = (u32)destination;
        d->phase_size = 96 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    }
    case 1:
        d->field_30.h.counter = index << 8;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        d->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        d->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs |= 0x10000;
        d->transfer_mode = 2;
        d->sector_destination = (u32)gFile_TransferBufferBase;
        d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;
    case 2:
        d->sector_destination = d->alternate_sector_destination = (u32)gFile_abPackageStagingBuffer;
        d->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 3: {
        u32 mask;
        u8 *source;
        RECT *rect;
        do { rect = &rect0; } while (0);
        rect0.x = MODEL_TEXTURE_STAGE3_RECT_X(index);
        rect0.y = MODEL_TEXTURE_STAGE3_RECT_Y(index);
        rect0.w = 0x100;
        rect0.h = 8;
        LoadImage2(rect, (u32 *)gFile_abPackageStagingBuffer);
        source = gFile_abPackageStagingBuffer;
        do { mask = 0xFFDCFFFF; } while (0);
        source += 2 * FILE_SECTOR_SIZE;
        d->sector_destination = d->alternate_sector_destination = (u32)source;
        d->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= mask;
        d->transfer_mode = 1;
        break;
    }
    case 4:
        if (alternate == 0) {
            rect1.x = MODEL_TEXTURE_PALETTE_RECT_X;
            rect1.y = index + 0xF2;
            rect1.w = 0x100;
            rect1.h = 1;
            LoadImage2(&rect1, (u32 *)gModel_abTexturePaletteStaging);
            d->field_30.h.counter = (index << 8) + 0xC0;
            d->w = 0x40;
            d->h = 0x10;
            {
                u32 cleared, loaded, enabled;
                cleared = gFile_dwTransferState_abs & 0xFFDDFFFF;
                gFile_dwTransferState_abs = cleared;
                do { loaded = gFile_dwTransferState_abs; } while (0);
                do { enabled = 0x10000; } while (0);
                d->field_30.h.field_32 = 0x100;
                gFile_dwTransferState_abs = loaded | enabled;
            }
            d->transfer_mode = 2;
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            d->sector_destination = (u32)gFile_TransferBufferBase;
            d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        } else {
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 6:
        if (alternate == 1) {
            rect1.x = MODEL_TEXTURE_PALETTE_RECT_X;
            rect1.y = index + 0xF2;
            rect1.w = 0x100;
            rect1.h = 1;
            LoadImage2(&rect1, (u32 *)gFile_abPackageStagingBuffer);
            d->field_30.h.counter = (index << 8) + 0xC0;
            d->w = 0x40;
            d->h = 0x10;
            {
                u32 cleared, loaded, enabled;
                cleared = gFile_dwTransferState_abs & 0xFFDDFFFF;
                gFile_dwTransferState_abs = cleared;
                do { loaded = gFile_dwTransferState_abs; } while (0);
                do { enabled = 0x10000; } while (0);
                d->field_30.h.field_32 = 0x100;
                gFile_dwTransferState_abs = loaded | enabled;
            }
            d->transfer_mode = 2;
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            d->sector_destination = (u32)gFile_TransferBufferBase;
            d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        } else {
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 7:
        if (alternate == 0 && index == 0) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->sector_destination = d->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
            d->transfer_mode = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 8:
        if (alternate == 0 && index == 1) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->sector_destination = d->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
            d->transfer_mode = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 9:
        if (alternate == 1 && index == 0) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->sector_destination = d->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
            d->transfer_mode = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 10:
        if (alternate == 1 && index == 1) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->sector_destination = d->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
            d->transfer_mode = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 11:
        if (index == 0) {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->sector_destination = d->alternate_sector_destination = (u32)gModel_pSlot0PrimaryModule;
            d->transfer_mode = 1;
        } else {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 12:
        if (index == 1) {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->sector_destination = d->alternate_sector_destination = (u32)gModel_pSlot1PrimaryModule;
            d->transfer_mode = 1;
        } else {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        }
        break;
    case 13:
        if (slot->skip_sound_setup != 0) {
            d->phase_size = FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        } else {
            d->sector_destination = d->alternate_sector_destination =
                (u32)(D_801A8000 + index * FILE_SECTOR_SIZE);
            d->phase_size = FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            d->transfer_mode = 1;
        }
        break;
    case 14:
        if (slot->skip_sound_setup != 0) {
            u32 mask;
            do { mask = 0xFFFCFFFF; } while (0);
            d->phase_size = 50 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= mask;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            d->transfer_mode = 1;
        } else {
            d->transfer_mode = 3;
            d->field_30.word = index * 0x19000 + 0xD810;
            d->phase_size = 50 * FILE_SECTOR_SIZE;
            d->sector_destination = (u32)gFile_TransferBufferBase;
            d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        }
        break;
    case 5:
    case 15:
        d->sector_destination = d->alternate_sector_destination = (u32)gFile_abPackageStagingBuffer;
        d->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 16: {
        u8 *source;
        source = gFile_abPackageStagingBuffer;
        Model_CopyWords(
            (s32 *)slot->sound_entries,
            (const s32 *)source,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotCF8BlockWords *)&slot->field_CF8 =
            ((ModelTransferMetadata *)source)->field_CF8;
        if (slot->field_CF8.prefix.values.field_08 & 0x20200000) {
            Model_SetSlotDrawMode(index, 4);
        }
        if (slot->field_DFA < 0xFFFF) {
            slot->field_CF8.prefix.values.field_00 = slot->field_DFA;
        }
        if (slot->field_DFC < 0xFFFF) {
            slot->field_CF8.prefix.values.field_02 = slot->field_DFC;
        }
        if (slot->skip_sound_setup != 0) {
            slot->field_CF8.field_10 = -1;
            slot->field_CF8.field_14 = -1;
            slot->field_CF8.field_18 = -1;
        }
        slot->load_step = 1;
        break;
    }
    }
}

/* Two transfer-phase callbacks of the FreeDuel_LoadPackageStage family, kept
   in one translation unit because they are the same routine over different
   assets: each takes the descriptor plus a phase index, and each switch arm
   programs the same fields -- phase_size, the sector_destination/alternate_sector_destination source
   window, done, and the field_30 halfword pair -- while clearing and setting
   the same bits of gFile_dwTransferState_abs.

   Both own a .rodata jump table, and the two tables are adjacent in the image
   (0x1F84 for the six-case switch, 0x1F9C for the eleven-case one). Together
   with Model_LoadMonsterMergeStage's 17-entry table and the retail zero word after it, the
   complete object owns the 0x8C-byte .rodata run at 0x1F3C. Definition order
   is address order and must stay that way. */

void Model_LoadAuxiliaryPackageStage(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;

    switch (mode) {
    case 0:
        object->phase_size = 18 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gModel_AuxiliaryPayloadAddress;
        object->sector_destination = gModel_AuxiliaryPayloadAddress;
        object->transfer_mode = 1;
        break;

    case 1:
        object->field_30.h.counter = 0x200;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 2:
        object->alternate_sector_destination = (s32)gModel_abAuxiliaryPaletteSector;
        object->sector_destination = (s32)gModel_abAuxiliaryPaletteSector;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        rect0.x = MODEL_TEXTURE_PALETTE_RECT_X;
        rect0.y = 0xF1;
        rect0.w = 0x100;
        rect0.h = 1;
        LoadImage2(&rect0, (u32 *)gModel_abAuxiliaryPaletteSector);
        object->alternate_sector_destination = (s32)(gModel_abAuxiliaryPaletteSector - FILE_SECTOR_SIZE);
        object->sector_destination = (s32)(gModel_abAuxiliaryPaletteSector - FILE_SECTOR_SIZE);
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 4:
        rect1.x = MODEL_TEXTURE_PALETTE_RECT_X;
        rect1.y = 0xF4;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)gFile_abPackageStagingBuffer);
        object->field_30.h.counter = 0x380;
        object->field_30.h.field_32 = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 5:
        gModel_bAuxiliaryLoadStep[0] = 1;
        break;
    }
}

void Model_LoadSpecialBattlePackageStage(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;
    ModelSlot *dst;
    u8 *src;

    switch (mode) {
    case 0:
        object->phase_size = 96 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->transfer_mode = 1;
        break;

    case 1:
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->field_30.h.counter = 0;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 96 * FILE_SECTOR_SIZE;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 2:
        object->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        object->sector_destination = (s32)gFile_abPackageStagingBuffer;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
#ifdef VERSION_EUROPE
        /* The European build stores the row's x first. */
        rect0.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
        rect0.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        rect0.w = 0x100;
#else
        rect0.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        rect0.w = 0x100;
        rect0.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
#endif
        rect0.h = 8;
        LoadImage2(&rect0, (u32 *)gFile_abPackageStagingBuffer);
        object->phase_size = 10 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
        object->sector_destination = gModel_Slot0VariantModuleAddress;
        object->transfer_mode = 1;
        break;

    case 4:
        object->phase_size = 10 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
        object->sector_destination = gModel_Slot1VariantModuleAddress;
        object->transfer_mode = 1;
        break;

    case 6:
        rect1.x = MODEL_TEXTURE_STAGE3_RECT_X(1);
        rect1.y = MODEL_TEXTURE_TAIL_ROW_Y;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)gFile_abPackageStagingBuffer);
        object->field_30.h.counter = 0x180;
        object->w = 0x40;
        object->field_30.h.field_32 = 0x100;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 8 * FILE_SECTOR_SIZE;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 7:
        object->alternate_sector_destination = (s32)D_801A8000;
        object->sector_destination = (s32)D_801A8000;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 8:
        object->transfer_mode = 3;
        object->field_30.word = 0xD810;
        object->phase_size = 50 * FILE_SECTOR_SIZE;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 5:
    case 9:
        object->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        object->sector_destination = (s32)gFile_abPackageStagingBuffer;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 10:
        dst = gModel_aSlots;
        src = gFile_abPackageStagingBuffer;
        Model_CopyWords(
            (s32 *)dst->sound_entries,
            (const s32 *)src,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotCF8BlockWords *)&dst->field_CF8 =
            ((ModelTransferMetadata *)src)->field_CF8;
        dst->field_CF8.field_10 = -1;
        dst->field_CF8.field_14 = -1;
        dst->field_CF8.field_18 = -1;
        dst->field_CF8.prefix.values.field_00 = 0;
        dst->field_CF8.prefix.values.field_02 = 0;
        dst->load_step = 1;
        break;
    }
}

/* Starts, restores or stops one model slot's part animation.
 *
 * -1 clears every part's command index. 0 restores the words the active row
 * saved, three per part or five for parts flagged in extended_animation_part_mask, and clears
 * the active row. Any other row saves the parts' current words, records each
 * part's command index and makes that row active, restoring the previous row
 * first when the caller allows it.
 *
 * Each loop keeps its own word count, row entry and rounded part index: the
 * part bit is found by shifting with the word count while it still holds 3,
 * which is the retail srav/sllv form, and a shared variable would give the
 * word count the slot's callee-saved register. */
void Model_ControlSlotAnimation(s32 index, s32 anim, s32 flag) {
    ModelSlot *m;
    ModelSlotPart *G32 *parts;
    u8 *dst;
    u8 *base;
    u16 *src;
    s32 i;
    s32 prev;
    s32 restore_words;
    s32 copy_words;
    s32 restore_row;
    s32 copy_row;
    s32 copy_bit;
    s32 restore_bit;

    m = &gModel_aSlots[index];
    dst = m->animation_snapshot_buffer;
    base = m->animation_parameter_base;
    parts = m->animation_sequences;
    if (m->is_active == 0) {
        return;
    }
    switch (anim) {
    case -1:
        m->part_command_reset_pending = 0;
        for (i = 0; i < m->animation_sequence_count; parts++, i++) {
            (*parts)->ii = 0xFFFF;
        }
        break;
    case 0:
        if (m->active_animation_command_row == 0) {
            return;
        }
        for (i = 0; i < m->animation_sequence_count; i++) {
            restore_row = m->animation_command_indices[m->active_animation_command_row][i];
            src = (u16 *)&m->animation_commands[restore_row];
            if (restore_row != 0xFFFF) {
                restore_bit = i;
                if (i < 0) {
                    restore_bit = i + 7;
                }
                restore_words = 3;
                if ((m->extended_animation_part_mask[restore_bit >> restore_words] >>
                     (i - ((restore_bit >> restore_words) << restore_words))) &
                    1) {
                    restore_words = 5;
                }
                Model_CopyWords((s32 *)(base + src[0] * 4), (const s32 *)dst,
                                restore_words);
                dst += restore_words * 4;
            }
        }
        m->active_animation_command_row = 0;
        break;
    default:
        prev = m->active_animation_command_row;
        if (anim != m->guardian_star_selection + 3) {
            m->animation_control_state = 2;
        }
        if (m->animation_duration_rows[anim].max == 0) {
            return;
        }
        if (prev != 0) {
            if (flag == 0) {
                return;
            }
            m->queued_animation_row_latch = m->queued_animation_row;
            Model_ControlSlotAnimation(index, 0, 0);
        }
        if (anim == m->guardian_star_selection + 3
            || m->model_id == MODEL_SPECIAL_BATTLE_ID) {
            m->animation_control_state = 7;
        }
        m->active_animation_command_row = anim;
        if (prev == anim) {
            Model_SeekSlotAnimation(index, 0, 0);
            return;
        }
        m->part_command_reset_pending = 1;
        for (i = 0; i < m->animation_sequence_count; parts++, i++) {
            copy_row = m->animation_command_indices[m->active_animation_command_row][i];
            src = (u16 *)&m->animation_commands[copy_row];
            if (copy_row != 0xFFFF) {
                copy_bit = i;
                if (i < 0) {
                    copy_bit = i + 7;
                }
                copy_words = 3;
                if ((m->extended_animation_part_mask[copy_bit >> copy_words] >>
                     (i - ((copy_bit >> copy_words) << copy_words))) &
                    1) {
                    copy_words = 5;
                }
                (*parts)->ii = ((u8 *)src - (u8 *)m->animation_commands) >> 2;
                Model_CopyWords((s32 *)dst,
                                (const s32 *)(base + src[0] * 4), copy_words);
                dst += copy_words * 4;
            }
        }
        break;
    }
}
