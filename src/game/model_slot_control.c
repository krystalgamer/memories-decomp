#define TEXTURE_TINT_RECT ((RECT *)&gModel_sTextureTintRectX)
#define TEXTURE_TINT_WORDS(buffer) ((u32 *)(buffer))
#include "../types.h"
#include "model_get_slot_dimensions_with_minimums.h"
#include "model.h"
#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "color_constants.h"
#include "color.h"
#include "model_apply_texture_tint.h"
#include "model_update_view_metrics.h"
#include "camera_view.h"
#include "model_step_view_orbit.h"

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
    ModelSlot *slot;
    s32 animation_row;
    s32 row_end_position;
    s32 position_step;
    s32 part_index;
    s32 scaled_position;
    s32 forward_handoff_position;

    slot = &gModel_aSlots[index];
    if (slot->is_active == 0) {
        return;
    }
    if (slot->animation_commands != 0) {
        animation_row = slot->current_animation_row;
        row_end_position = slot->animation_duration_rows[animation_row].max << 4;
        position_step = slot->playback_speed * Model_GetFrameStep();
        if (animation_row == 6) {
            forward_handoff_position = row_end_position - position_step;
            if ((slot->playback_state == 0x3E && slot->animation_position >= forward_handoff_position) ||
                (slot->playback_state == 0x3C && slot->animation_position < 2)) {
                Model_SetSlotPlaybackDirection(index, 0);
            }
        }
        if (index < 2) {
            Model_PlaySlotAnimationSounds(index);
        }
        if (slot->playback_state != 0x23) {
            if (slot->playback_state == 0x3E) {
                s32 forward_position;

                if (slot->animation_position >= row_end_position) {
                    slot->animation_position = 0;
                }
                forward_position = slot->animation_position + position_step;
                if (forward_position >= row_end_position) {
                    slot->animation_position = row_end_position;
                } else {
                    slot->animation_position = forward_position;
                }
            } else {
                s32 reverse_position;

                if (slot->animation_position == 0) {
                    slot->animation_position = row_end_position;
                }
                reverse_position = slot->animation_position - position_step;
                if (reverse_position <= 0) {
                    slot->animation_position = 0;
                } else {
                    slot->animation_position = reverse_position;
                }
            }
        }
        if (slot->part_command_reset_pending == 0) {
            if (slot->playback_state == 0x3E ? slot->animation_position >= row_end_position : slot->animation_position == 0) {
                if (slot->tint[7] == 6) {
                    slot->queued_animation_row = 2;
                }
                if (slot->queued_animation_row != slot->queued_animation_row_latch) {
                    Model_SeekSlotAnimation(index, slot->queued_animation_row, 0);
                    slot->queued_animation_row_latch = slot->queued_animation_row;
                    if (slot->active_animation_command_row != 0) {
                        Model_ControlSlotAnimation(index, 0, 0);
                    }
                    if (slot->tint[7] == 6) {
                        Model_QueueEffectEndpointPair(-1, index + 2, slot->animation_duration_rows[2].max);
                        slot->tint[7] = 0;
                        slot->queued_animation_row = 1;
                    }
                }
            }
        }
    }
    if (slot->playback_state != 0x23) {
        if (slot->playback_speed == 0x10) {
            if (Model_GetFrameStep() == 2) {
                return;
            }
        }
    }
    scaled_position = slot->animation_position;
    if (slot->animation_sequence_count != 0) {
        part_index = 0;
        do {
            Model_SeekSlotPartAnimation(slot, part_index, slot->current_animation_row, scaled_position);
            part_index++;
        } while (part_index < slot->animation_sequence_count);
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
        stat_base = base + (u32)&((ModelSlot *)0)->metadata;
        own_record = own_offset + stat_base;
        other_record = other_offset + stat_base;
        if (other->defense_position != 0) {
            {
                u16 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
                u16 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.defense_compare_value;
                selected = own_value > other_value ? 6 : 8;
            }
        } else {
            u16 own_value =
                MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
            u16 other_value =
                MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.attack_compare_value;
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
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
                u32 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.defense_compare_value;
                reaction = other_value < own_value ? 6 : 8;
            }
        } else {
            u32 own_value =
                MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
            u32 other_value =
                MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.attack_compare_value;
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
                                prefix.values.attack_compare_value;
                        u32 other_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                                prefix.values.defense_compare_value;
                        fallback_action = other_value < own_value ? 6 : 8;
                    }
                } else {
                    u32 own_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                            prefix.values.attack_compare_value;
                    u32 other_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                            prefix.values.attack_compare_value;
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
    p->defense_compare_override = 0xFFFF;
    p->attack_compare_override = 0xFFFF;

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

    p->metadata.first_star_module_command = -1;
    p->metadata.second_star_module_command = -1;
    p->metadata.primary_module_command = -1;
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
    ModelSlot *span_slot;
    ModelSlot *rearm_slot;
    ModelSlotPart *G32 *part_sequence;
    ModelSlot *draw_slot;
    ModelSlot *tint_slot;
    s32 span_part_index;
    s32 rearm_part_index;
    s32 snapshot_bytes;
    s32 mask_byte_index;
    s32 cache_or_speed;
    s32 missing_command_index;
    s32 part_count;
    u8 *slot_bytes;
    u8 *tint_slot_bytes;
    s32 part_speed;

    Model_InitSlot(arg0, arg1, arg2, arg3);
    if (arg1 == (u8 *)0) {
        return;
    }
    Model_BuildSlotAnimationTables(arg0);
    Model_LinkSlotAnimationRows(arg0);
    span_slot = &gModel_aSlots[arg0];
    snapshot_bytes = 0;
    if (arg0 < 2) {
        span_part_index = snapshot_bytes;
        if (snapshot_bytes < span_slot->animation_sequence_count) {
            missing_command_index = 0xFFFF;
            part_count = span_slot->animation_sequence_count;
            do {
                if (span_slot->animation_command_indices[1][span_part_index] != missing_command_index) {
                    mask_byte_index = span_part_index / 8;
                    if ((span_slot->extended_animation_part_mask[mask_byte_index] >> (span_part_index - mask_byte_index * 8)) & 1) {
                        snapshot_bytes += 0x14;
                    } else {
                        snapshot_bytes += 0xC;
                    }
                }
                span_part_index++;
            } while (span_part_index < part_count);
        }
    }
    cache_or_speed = (span_slot->lit_color_cache_address = (s32)span_slot->animation_snapshot_buffer + snapshot_bytes);
    span_slot->lit_color_cache_end_address = cache_or_speed + span_slot->lit_color_cache_entry_count * sizeof(u32);
    Model_SetSlotRootTransform(span_slot, 0, 0, 0, arg0 == 1 ? 0x800 : 0);

    rearm_slot = &gModel_aSlots[arg0];
    part_sequence = rearm_slot->animation_sequences;
    for (rearm_part_index = 0; rearm_part_index < rearm_slot->animation_sequence_count; rearm_part_index++) {
        (*part_sequence)->ii = 0xFFFF;
        (*part_sequence)->ti = (*part_sequence)->start;
        (*part_sequence)->aframe = 0xFFFF;
        (*part_sequence)->sid = (*part_sequence)->start_sid;
        (*part_sequence)->rframe = 0;
        (*part_sequence)->speed = 0x10;
        part_sequence++;
    }

    slot_bytes = (u8 *)gModel_aSlots;
    draw_slot = (ModelSlot *)(arg0 * MODEL_SLOT_SIZE + slot_bytes);
    draw_slot->is_active = 1;
    Model_DrawSlotUnitsOnly(arg0);
    cache_or_speed = draw_slot->playback_speed * 2;
    part_speed = cache_or_speed;
    switch (draw_slot->playback_state) {
    case 0x23:
        part_speed = 0;
        Model_SetSlotPartSpeeds(arg0, part_speed);
        break;
    case 0x3E:
        part_speed = cache_or_speed;
        Model_SetSlotPartSpeeds(arg0, part_speed);
        break;
    case 0x3C:
        part_speed = -part_speed;
        Model_SetSlotPartSpeeds(arg0, part_speed);
        break;
    }
    if (arg0 < 2) {
        tint_slot_bytes = (u8 *)gModel_aSlots;
        tint_slot = (ModelSlot *)(arg0 * MODEL_SLOT_SIZE + tint_slot_bytes);
        Model_ApplyTextureTint(arg0, tint_slot->texture_tint_control, tint_slot->texture_tint_scale);
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
s32 Model_LoadMonsterMerge(s32 slot, s32 model, s32 attack_override, s32 defense_override, s32 guardian_star_option,
                           s32 defense_position_option, s32 arg6)
{
    FileTransferDescriptor *transfer;
    s32 skip_sound_setup_mask;
    s32 request_result;

    request_result = 0;
    skip_sound_setup_mask = slot & 0x80;
    slot &= 0x7F;
    if (model < 0) {
        model = gModel_aSlots[slot].model_id;
    } else {
        gModel_aSlots[slot].model_id = model;
    }
    if (skip_sound_setup_mask == 0) {
        SD_KeyOffVoiceSlots();
    }
    File_WaitForTransfers();
    Model_InitSlot(slot, 0, 0, arg6);
    gModel_aSlots[slot].skip_sound_setup = skip_sound_setup_mask;
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
            request_result++;
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
            if (attack_override >= 0) {
                gModel_aSlots[slot].attack_compare_override = attack_override;
            }
            if (defense_override >= 0) {
                gModel_aSlots[slot].defense_compare_override = defense_override;
            }
            if (guardian_star_option >= 0) {
                gModel_aSlots[slot].guardian_star_selection = guardian_star_option != 0;
            }
            if (defense_position_option >= 0) {
                gModel_aSlots[slot].defense_position = defense_position_option != 0;
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
        request_result++;
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
    return request_result;
}

static inline s16 load_phase(ModelSlot *slot)
{
    return slot->load_step;
}

void Model_AdvanceSlotLoad(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];
#ifndef VERSION_JAPAN
    s32 start_vblank = VSync(1);
#endif
    s32 initial_load_step = slot->load_step;
    s32 phase_work;
    s32 phase_value;
    s32 part_speed;

    if (initial_load_step == 0 || initial_load_step == 255) {
        return;
    }
    switch (load_phase(slot)) {
    case 1: {
        u8 *hmd_payload = (u8 *)gModel_AuxiliaryPayloadAddress;
        s32 hmd_size = 0xC000;
        switch (index) {
        case 0:
            hmd_payload = gRuntime_pSharedWorkBase;
            break;
        case 1:
            hmd_payload = gModel_pSlot1PayloadBase;
            break;
        }
        if (*(s32 *)hmd_payload != 0) {
            hmd_size = *(s32 *)hmd_payload;
        }
        Model_InitSlot(index, hmd_payload, hmd_size, -1);
        break;
    }
    case 2:
        Model_BuildSlotAnimationTables(index);
        break;
    case 3:
        Model_LinkSlotAnimationRows(index);
        break;
    case 4: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        s32 snapshot_bytes = 0;
        s32 part_index, part_count, mask_byte_index, missing_command_index;
        if (index < 2) {
            part_index = snapshot_bytes;
            if (snapshot_bytes < phase_slot->animation_sequence_count) {
                missing_command_index = 0xFFFF;
                part_count = phase_slot->animation_sequence_count;
                do {
                    if (phase_slot->animation_command_indices[1][part_index] != missing_command_index) {
                        mask_byte_index = part_index / 8;
                        if ((phase_slot->extended_animation_part_mask[mask_byte_index] >> (part_index - mask_byte_index * 8)) & 1) {
                            snapshot_bytes += 0x14;
                        } else {
                            snapshot_bytes += 0xC;
                        }
                    }
                    part_index++;
                } while (part_index < part_count);
            }
        }
        phase_value = (phase_slot->lit_color_cache_address = (s32)phase_slot->animation_snapshot_buffer + snapshot_bytes);
        phase_slot->lit_color_cache_end_address = phase_value + phase_slot->lit_color_cache_entry_count * 4;
        Model_SetSlotRootTransform(phase_slot, 0, 0, 0, index == 1 ? 0x800 : 0);
        break;
    }
    case 5: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        ModelSlotPart *G32 *part_sequence = phase_slot->animation_sequences;
        s32 part_index;
        for (part_index = 0; part_index < phase_slot->animation_sequence_count; part_index++) {
            (*part_sequence)->ii = 0xFFFF;
            (*part_sequence)->ti = (*part_sequence)->start;
            (*part_sequence)->aframe = 0xFFFF;
            (*part_sequence)->sid = (*part_sequence)->start_sid;
            (*part_sequence)->rframe = 0;
            (*part_sequence)->speed = 0x10;
            part_sequence++;
        }
        break;
    }
    case 6:
        slot->is_active = 1;
        Model_DrawSlotUnitsOnly(index);
        slot->is_active = 0;
        break;
    case 7: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        phase_value = phase_slot->playback_speed * 2;
        part_speed = phase_value;
        switch (phase_slot->playback_state) {
        case 0x23:
            part_speed = 0;
            Model_SetSlotPartSpeeds(index, part_speed);
            break;
        case 0x3E:
            part_speed = phase_value;
            Model_SetSlotPartSpeeds(index, part_speed);
            break;
        case 0x3C:
            part_speed = -part_speed;
            Model_SetSlotPartSpeeds(index, part_speed);
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
        ModelSlot *phase_slot = &gModel_aSlots[index];
        u8 *handler_context;
        ModelControlHandler module_handler;
        s32 selected_row;
        s32 frame_position;
        s32 part_index;
        phase_work = ((ModelControlCommandView *)phase_slot)->commands[2];
        handler_context = (u8 *)phase_slot->primary_module_data_address;
        gModel_bActiveSlotIndex = index;
        if (phase_work >= 0) {
            if (index != 0) {
                module_handler = (ModelControlHandler)(gModel_pSlot1PrimaryModule + 4);
            } else {
                module_handler = (ModelControlHandler)(gModel_pSlot0PrimaryModule + 4);
            }
            Model_SetHandlerCallbackState(1);
            module_handler(handler_context, ((ModelControlCommandView *)phase_slot)->commands[2] % 1000);
            Model_SetHandlerCallbackState(0);
        }
        selected_row = phase_slot->current_animation_row;
        phase_slot->animation_control_state = 6;
        frame_position = phase_slot->animation_position;
        for (part_index = 0; part_index < phase_slot->animation_sequence_count; part_index++) {
            s32 current_row = phase_slot->current_animation_row;
            if (selected_row != 0) {
                phase_slot->animation_sequences[part_index]->sid = selected_row;
                current_row = selected_row;
            }
            Model_SeekSlotPartAnimation(phase_slot, part_index, current_row, frame_position);
        }
        if (selected_row != 0) {
            phase_slot->current_animation_row = selected_row;
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
    phase_work = VSync(1);
    printf(gModel_szLoadModelTraceFormat, slot->load_step, phase_work - start_vblank);
#endif
    {
        s32 next_load_step;
        if (slot->is_active != 0) {
            next_load_step = 255;
        } else {
            next_load_step = slot->load_step + 1;
        }
        slot->load_step = next_load_step;
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

void Model_LoadMonsterMergeStage(FileTransferDescriptor *transfer, s32 stage)
{
    RECT texture_row_rect;
    RECT palette_row_rect;
    s32 slot_index;
    s32 variant_selection;
    ModelSlot *loaded_slot;

    slot_index = (s32)transfer->callback_data;
    variant_selection = transfer->position;
    loaded_slot = &gModel_aSlots[slot_index];
    switch (stage) {
    case 0: {
        u8 *payload_destination;
        if (slot_index == 0) {
            payload_destination = gRuntime_pSharedWorkBase;
        } else {
            payload_destination = gModel_pSlot1PayloadBase;
        }
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)payload_destination;
        transfer->phase_size = 96 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;
    }
    case 1:
        transfer->field_30.h.counter = slot_index << 8;
        transfer->field_30.h.field_32 = 0x100;
        transfer->w = 0x40;
        transfer->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        transfer->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;
    case 2:
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)gFile_abPackageStagingBuffer;
        transfer->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;
    case 3: {
        u32 transfer_clear_mask;
        u8 *next_sector_source;
        RECT *texture_row_view;
        do { texture_row_view = &texture_row_rect; } while (0);
        texture_row_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(slot_index);
        texture_row_rect.y = MODEL_TEXTURE_STAGE3_RECT_Y(slot_index);
        texture_row_rect.w = 0x100;
        texture_row_rect.h = 8;
        LoadImage2(texture_row_view, (u32 *)gFile_abPackageStagingBuffer);
        next_sector_source = gFile_abPackageStagingBuffer;
        do { transfer_clear_mask = 0xFFDCFFFF; } while (0);
        next_sector_source += 2 * FILE_SECTOR_SIZE;
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)next_sector_source;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= transfer_clear_mask;
        transfer->transfer_mode = 1;
        break;
    }
    case 4:
        if (variant_selection == 0) {
            palette_row_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
            palette_row_rect.y = slot_index + 0xF2;
            palette_row_rect.w = 0x100;
            palette_row_rect.h = 1;
            LoadImage2(&palette_row_rect, (u32 *)gModel_abTexturePaletteStaging);
            transfer->field_30.h.counter = (slot_index << 8) + 0xC0;
            transfer->w = 0x40;
            transfer->h = 0x10;
            {
                u32 cleared_transfer_state, reloaded_transfer_state, image_transfer_bit;
                cleared_transfer_state = gFile_dwTransferState_abs & 0xFFDDFFFF;
                gFile_dwTransferState_abs = cleared_transfer_state;
                do { reloaded_transfer_state = gFile_dwTransferState_abs; } while (0);
                do { image_transfer_bit = 0x10000; } while (0);
                transfer->field_30.h.field_32 = 0x100;
                gFile_dwTransferState_abs = reloaded_transfer_state | image_transfer_bit;
            }
            transfer->transfer_mode = 2;
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            transfer->sector_destination = (u32)gFile_TransferBufferBase;
            transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        } else {
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 6:
        if (variant_selection == 1) {
            palette_row_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
            palette_row_rect.y = slot_index + 0xF2;
            palette_row_rect.w = 0x100;
            palette_row_rect.h = 1;
            LoadImage2(&palette_row_rect, (u32 *)gFile_abPackageStagingBuffer);
            transfer->field_30.h.counter = (slot_index << 8) + 0xC0;
            transfer->w = 0x40;
            transfer->h = 0x10;
            {
                u32 cleared_transfer_state, reloaded_transfer_state, image_transfer_bit;
                cleared_transfer_state = gFile_dwTransferState_abs & 0xFFDDFFFF;
                gFile_dwTransferState_abs = cleared_transfer_state;
                do { reloaded_transfer_state = gFile_dwTransferState_abs; } while (0);
                do { image_transfer_bit = 0x10000; } while (0);
                transfer->field_30.h.field_32 = 0x100;
                gFile_dwTransferState_abs = reloaded_transfer_state | image_transfer_bit;
            }
            transfer->transfer_mode = 2;
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            transfer->sector_destination = (u32)gFile_TransferBufferBase;
            transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        } else {
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 7:
        if (variant_selection == 0 && slot_index == 0) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 8:
        if (variant_selection == 0 && slot_index == 1) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 9:
        if (variant_selection == 1 && slot_index == 0) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 10:
        if (variant_selection == 1 && slot_index == 1) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 11:
        if (slot_index == 0) {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = (u32)gModel_pSlot0PrimaryModule;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 12:
        if (slot_index == 1) {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = (u32)gModel_pSlot1PrimaryModule;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 13:
        if (loaded_slot->skip_sound_setup != 0) {
            transfer->phase_size = FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFFCFFFF;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        } else {
            transfer->sector_destination = transfer->alternate_sector_destination =
                (u32)(D_801A8000 + slot_index * FILE_SECTOR_SIZE);
            transfer->phase_size = FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= 0xFFDCFFFF;
            transfer->transfer_mode = 1;
        }
        break;
    case 14:
        if (loaded_slot->skip_sound_setup != 0) {
            u32 transfer_clear_mask;
            do { transfer_clear_mask = 0xFFFCFFFF; } while (0);
            transfer->phase_size = 50 * FILE_SECTOR_SIZE;
            gFile_dwTransferState_abs &= transfer_clear_mask;
            gFile_dwTransferState_abs |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        } else {
            transfer->transfer_mode = 3;
            transfer->field_30.word = slot_index * 0x19000 + 0xD810;
            transfer->phase_size = 50 * FILE_SECTOR_SIZE;
            transfer->sector_destination = (u32)gFile_TransferBufferBase;
            transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        }
        break;
    case 5:
    case 15:
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)gFile_abPackageStagingBuffer;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;
    case 16: {
        u8 *metadata_source;
        metadata_source = gFile_abPackageStagingBuffer;
        Model_CopyWords(
            (s32 *)loaded_slot->sound_entries,
            (const s32 *)metadata_source,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotMetadataBlockWords *)&loaded_slot->metadata =
            ((ModelTransferMetadata *)metadata_source)->metadata;
        if (loaded_slot->metadata.prefix.values.field_08 & 0x20200000) {
            Model_SetSlotDrawMode(slot_index, 4);
        }
        if (loaded_slot->attack_compare_override < 0xFFFF) {
            loaded_slot->metadata.prefix.values.attack_compare_value = loaded_slot->attack_compare_override;
        }
        if (loaded_slot->defense_compare_override < 0xFFFF) {
            loaded_slot->metadata.prefix.values.defense_compare_value = loaded_slot->defense_compare_override;
        }
        if (loaded_slot->skip_sound_setup != 0) {
            loaded_slot->metadata.first_star_module_command = -1;
            loaded_slot->metadata.second_star_module_command = -1;
            loaded_slot->metadata.primary_module_command = -1;
        }
        loaded_slot->load_step = 1;
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

void Model_LoadAuxiliaryPackageStage(FileTransferDescriptor *transfer, s32 stage) {
    RECT first_palette_rect;
    RECT second_palette_rect;

    switch (stage) {
    case 0:
        transfer->phase_size = 18 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = gModel_AuxiliaryPayloadAddress;
        transfer->sector_destination = gModel_AuxiliaryPayloadAddress;
        transfer->transfer_mode = 1;
        break;

    case 1:
        transfer->field_30.h.counter = 0x200;
        transfer->field_30.h.field_32 = 0x100;
        transfer->w = 0x40;
        transfer->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 64 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 2:
        transfer->alternate_sector_destination = (s32)gModel_abAuxiliaryPaletteSector;
        transfer->sector_destination = (s32)gModel_abAuxiliaryPaletteSector;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 3:
        first_palette_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
        first_palette_rect.y = 0xF1;
        first_palette_rect.w = 0x100;
        first_palette_rect.h = 1;
        LoadImage2(&first_palette_rect, (u32 *)gModel_abAuxiliaryPaletteSector);
        transfer->alternate_sector_destination = (s32)(gModel_abAuxiliaryPaletteSector - FILE_SECTOR_SIZE);
        transfer->sector_destination = (s32)(gModel_abAuxiliaryPaletteSector - FILE_SECTOR_SIZE);
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 4:
        second_palette_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
        second_palette_rect.y = 0xF4;
        second_palette_rect.w = 0x100;
        second_palette_rect.h = 2;
        LoadImage2(&second_palette_rect, (u32 *)gFile_abPackageStagingBuffer);
        transfer->field_30.h.counter = 0x380;
        transfer->field_30.h.field_32 = 0;
        transfer->w = 0x40;
        transfer->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 32 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 5:
        gModel_bAuxiliaryLoadStep[0] = 1;
        break;
    }
}

void Model_LoadSpecialBattlePackageStage(FileTransferDescriptor *transfer, s32 stage) {
    RECT texture_row_rect;
    RECT palette_rows_rect;
    ModelSlot *slot_zero;
    u8 *metadata_source;

    switch (stage) {
    case 0:
        transfer->phase_size = 96 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        transfer->sector_destination = (s32)gRuntime_pSharedWorkBase;
        transfer->transfer_mode = 1;
        break;

    case 1:
        transfer->field_30.h.field_32 = 0x100;
        transfer->w = 0x40;
        transfer->field_30.h.counter = 0;
        transfer->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 96 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 2:
        transfer->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 3:
#ifdef VERSION_EUROPE
        /* The European build stores the row's x first. */
        texture_row_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
        texture_row_rect.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        texture_row_rect.w = 0x100;
#else
        texture_row_rect.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        texture_row_rect.w = 0x100;
        texture_row_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
#endif
        texture_row_rect.h = 8;
        LoadImage2(&texture_row_rect, (u32 *)gFile_abPackageStagingBuffer);
        transfer->phase_size = 10 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
        transfer->sector_destination = gModel_Slot0VariantModuleAddress;
        transfer->transfer_mode = 1;
        break;

    case 4:
        transfer->phase_size = 10 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
        transfer->sector_destination = gModel_Slot1VariantModuleAddress;
        transfer->transfer_mode = 1;
        break;

    case 6:
        palette_rows_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(1);
        palette_rows_rect.y = MODEL_TEXTURE_TAIL_ROW_Y;
        palette_rows_rect.w = 0x100;
        palette_rows_rect.h = 2;
        LoadImage2(&palette_rows_rect, (u32 *)gFile_abPackageStagingBuffer);
        transfer->field_30.h.counter = 0x180;
        transfer->w = 0x40;
        transfer->field_30.h.field_32 = 0x100;
        transfer->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 8 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 7:
        transfer->alternate_sector_destination = (s32)D_801A8000;
        transfer->sector_destination = (s32)D_801A8000;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 8:
        transfer->transfer_mode = 3;
        transfer->field_30.word = 0xD810;
        transfer->phase_size = 50 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 5:
    case 9:
        transfer->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 10:
        slot_zero = gModel_aSlots;
        metadata_source = gFile_abPackageStagingBuffer;
        Model_CopyWords(
            (s32 *)slot_zero->sound_entries,
            (const s32 *)metadata_source,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotMetadataBlockWords *)&slot_zero->metadata =
            ((ModelTransferMetadata *)metadata_source)->metadata;
        slot_zero->metadata.first_star_module_command = -1;
        slot_zero->metadata.second_star_module_command = -1;
        slot_zero->metadata.primary_module_command = -1;
        slot_zero->metadata.prefix.values.attack_compare_value = 0;
        slot_zero->metadata.prefix.values.defense_compare_value = 0;
        slot_zero->load_step = 1;
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
    ModelSlot *slot;
    ModelSlotPart *G32 *part_sequences;
    u8 *snapshot_cursor;
    u8 *parameter_base;
    u16 *command_halfwords;
    s32 part_index;
    s32 previous_row;
    s32 restore_word_count;
    s32 save_word_count;
    s32 restore_command_index;
    s32 save_command_index;
    s32 save_rounded_part;
    s32 restore_rounded_part;

    slot = &gModel_aSlots[index];
    snapshot_cursor = slot->animation_snapshot_buffer;
    parameter_base = slot->animation_parameter_base;
    part_sequences = slot->animation_sequences;
    if (slot->is_active == 0) {
        return;
    }
    switch (anim) {
    case -1:
        slot->part_command_reset_pending = 0;
        for (part_index = 0; part_index < slot->animation_sequence_count; part_sequences++, part_index++) {
            (*part_sequences)->ii = 0xFFFF;
        }
        break;
    case 0:
        if (slot->active_animation_command_row == 0) {
            return;
        }
        for (part_index = 0; part_index < slot->animation_sequence_count; part_index++) {
            restore_command_index = slot->animation_command_indices[slot->active_animation_command_row][part_index];
            command_halfwords = (u16 *)&slot->animation_commands[restore_command_index];
            if (restore_command_index != 0xFFFF) {
                restore_rounded_part = part_index;
                if (part_index < 0) {
                    restore_rounded_part = part_index + 7;
                }
                restore_word_count = 3;
                if ((slot->extended_animation_part_mask[restore_rounded_part >> restore_word_count] >>
                     (part_index - ((restore_rounded_part >> restore_word_count) << restore_word_count))) &
                    1) {
                    restore_word_count = 5;
                }
                Model_CopyWords((s32 *)(parameter_base + command_halfwords[0] * 4), (const s32 *)snapshot_cursor,
                                restore_word_count);
                snapshot_cursor += restore_word_count * 4;
            }
        }
        slot->active_animation_command_row = 0;
        break;
    default:
        previous_row = slot->active_animation_command_row;
        if (anim != slot->guardian_star_selection + 3) {
            slot->animation_control_state = 2;
        }
        if (slot->animation_duration_rows[anim].max == 0) {
            return;
        }
        if (previous_row != 0) {
            if (flag == 0) {
                return;
            }
            slot->queued_animation_row_latch = slot->queued_animation_row;
            Model_ControlSlotAnimation(index, 0, 0);
        }
        if (anim == slot->guardian_star_selection + 3
            || slot->model_id == MODEL_SPECIAL_BATTLE_ID) {
            slot->animation_control_state = 7;
        }
        slot->active_animation_command_row = anim;
        if (previous_row == anim) {
            Model_SeekSlotAnimation(index, 0, 0);
            return;
        }
        slot->part_command_reset_pending = 1;
        for (part_index = 0; part_index < slot->animation_sequence_count; part_sequences++, part_index++) {
            save_command_index = slot->animation_command_indices[slot->active_animation_command_row][part_index];
            command_halfwords = (u16 *)&slot->animation_commands[save_command_index];
            if (save_command_index != 0xFFFF) {
                save_rounded_part = part_index;
                if (part_index < 0) {
                    save_rounded_part = part_index + 7;
                }
                save_word_count = 3;
                if ((slot->extended_animation_part_mask[save_rounded_part >> save_word_count] >>
                     (part_index - ((save_rounded_part >> save_word_count) << save_word_count))) &
                    1) {
                    save_word_count = 5;
                }
                (*part_sequences)->ii = ((u8 *)command_halfwords - (u8 *)slot->animation_commands) >> 2;
                Model_CopyWords((s32 *)snapshot_cursor,
                                (const s32 *)(parameter_base + command_halfwords[0] * 4), save_word_count);
                snapshot_cursor += save_word_count * 4;
            }
        }
        break;
    }
}

/* Copies the four cached halfwords at record[idx]+0xDC8 into *out, then
   clamps each axis up to the per-record minimum the slot's
   CF8 prefix carries for it: min_x, min_y and min_z, each a byte multiplied
   by 16, with 0 meaning "no clamp". Finally sets out->max to the largest of
   {x (if positive), y, z}. */
void Model_GetSlotDimensionsWithMinimums(s32 idx, ModelEffectAdjustment *out) {
    ModelSlot *dimension_slot = &gModel_aSlots[idx];
    ModelSlotMetadataPrefix *metadata_prefix = &dimension_slot->metadata.prefix;

    *out = *(ModelEffectAdjustment *)dimension_slot->cached_dimensions;

    if (metadata_prefix->thresholds.min_x != 0) {
        s32 minimum_x = metadata_prefix->thresholds.min_x << 4;
        if (out->x < minimum_x) {
            out->x = minimum_x;
        }
    }
    if (metadata_prefix->thresholds.min_y != 0) {
        s32 minimum_y = metadata_prefix->thresholds.min_y << 4;
        if (out->y < minimum_y) {
            out->y = minimum_y;
        }
    }
    if (metadata_prefix->thresholds.min_z != 0) {
        s32 minimum_z = metadata_prefix->thresholds.min_z << 4;
        if (out->z < minimum_z) {
            out->z = minimum_z;
        }
    }

    out->max = 0;
    if (out->x > 0) {
        out->max = out->x;
    }
    if (out->max < out->y) {
        out->max = out->y;
    }
    if (out->max < out->z) {
        out->max = out->z;
    }
}

/* The first of the two steps of the camera-relative model transform.
 *
 * Model_UpdateViewMetrics recomputes the cached distance, yaw and pitch of
 * the current view. Model_BuildViewRelativeCoordinateUnit combines those
 * angles with a selected coordinate entry, retaining its slot-zero rotation
 * reads and lifetime-sensitive automatic parent link.
 *
 * The view is a GsRVIEW2 and the six offsets read here are its members:
 * 0x00, 0x04 and 0x08 are vpx, vpy and vpz, 0x0C, 0x10 and 0x14 are vrx,
 * vry and vrz. Three things say so -- this function already casts m to
 * GsRVIEW2 * to copy it whole into gModel_RView, camera_view.h declares that
 * global as GsRVIEW2, and the arithmetic reads as the standard camera
 * derivation: the horizontal distance and the yaw both take vp - vr in x
 * and z, and the pitch takes the y pair against that distance.
 *
 * They are read as *(s32 *)&view->vpx rather than view->vpx, and that is
 * measured. The plain member reads keep the instruction count at 226 but
 * rename registers across the whole function. Taking each member's address
 * keeps every read a scalar reference, as the old (m + offset) casts were,
 * and builds byte-identically. It is the same device
 * the two DisplayObject_Render*GouraudQuadList functions use; they too store to
 * fixed addresses between the reads.
 */
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "model.h"
#include "model_view_coordinate_helpers.h"

void Model_UpdateViewMetrics(GsRVIEW2 *m) {
    GsRVIEW2 *cached_view;
    GsRVIEW2 *selected_view;

    cached_view = &gModel_RView;
    if (m == 0) {
        m = cached_view;
    } else if (cached_view != 0 && m != cached_view) {
        gModel_RView = *m;
    }
    selected_view = m;

    *(s16 *)&gModel_wViewDistance = SquareRoot0((*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) * (*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) + (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz) * (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz));

    gModel_wViewYaw = ratan2(*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz,
                        *(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx);

    gModel_wViewPitch = ratan2(*(s32 *)&selected_view->vpy - *(s32 *)&selected_view->vry,
                        *(s16 *)&gModel_wViewDistance);

    *(s16 *)&gModel_wViewDistance = SquareRoot0((*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) * (*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) + (*(s32 *)&selected_view->vpy - *(s32 *)&selected_view->vry) * (*(s32 *)&selected_view->vpy - *(s32 *)&selected_view->vry) + (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz) * (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz));

    gModel_wViewYaw = (*(s16 *)&gModel_wViewYaw + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
    gModel_wViewPitch = (*(s16 *)&gModel_wViewPitch + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
}

static __inline__ void setup_rotation(
    SVECTOR *source,
    SVECTOR *copy,
    s32 turn,
    GsCOORDUNIT *unit
)
{
    source->vy = turn - gModel_wViewYaw;
    source->vz = gModel_wViewPitch;
    *copy = *source;

    unit->flg = 0;
    unit->rot.vx =
        turn - ((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vx;
    unit->rot.vy =
        turn - ((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vy;
    unit->rot.vz =
        turn - (u16)((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vz;
    RotMatrixZXY(&unit->rot, &unit->matrix);
}

static __inline__ void build_work_matrix(SVECTOR *rotation, MATRIX *work)
{
    RotMatrix_gte(rotation, work);
}

void Model_BuildViewRelativeCoordinateUnit(s32 index, s32 entry_index, SVECTOR *local_position, GsCOORDUNIT *output_coordinate)
{
    GsCOORDUNIT translation_parent;
    SVECTOR world_position;
    MATRIX entry_world_matrix;
    SVECTOR view_rotation_copy;
    SVECTOR view_rotation;
    MATRIX view_work_matrix;
    u8 transform_flags[8];
    ModelSlot *slot;

    slot = &gModel_aSlots[index];
    if (slot->entry_count < entry_index) {
        entry_index = slot->root_coordinate_index;
    }

    GsGetLwUnit(
        (GsCOORDUNIT *)(slot->entries + entry_index * MODEL_SLOT_DATA_ENTRY_SIZE),
        &entry_world_matrix
    );
    GsSetLsMatrix(&entry_world_matrix);

    RotTransSV(local_position, &world_position, (PSXLONG *)transform_flags);

    translation_parent.rot.vz = 0;
    translation_parent.rot.vy = 0;
    translation_parent.rot.vx = 0;
    RotMatrix_gte(&translation_parent.rot, &translation_parent.matrix);

    translation_parent.flg = 1;
    translation_parent.matrix.t[0] = world_position.vx;
    translation_parent.matrix.t[1] = world_position.vy;
    translation_parent.matrix.t[2] = world_position.vz;
    translation_parent.workm = translation_parent.matrix;
    translation_parent.super = NULL;
    memset(&view_rotation, 0, sizeof(view_rotation));

    setup_rotation(&view_rotation, &view_rotation_copy, MODEL_ANGLE_FULL_TURN, output_coordinate);

    build_work_matrix(&view_rotation_copy, &view_work_matrix);
    MulMatrix(&output_coordinate->matrix, &view_work_matrix);

    output_coordinate->matrix.t[2] = 0;
    output_coordinate->matrix.t[1] = 0;
    output_coordinate->matrix.t[0] = 0;
    output_coordinate->super = &translation_parent;
}

#ifdef VERSION_EUROPE
void Model_ApplyTextureTint(s32 side_index, s32 tint_control, s32 saturation_scale)
{
    /* The European build keeps each side's two rows at x 0x280, y
       (side_index << 4) + 0xD0 and 0xD8, where the US build has x side_index << 8 and
       y 0xF0 and 0xF8. */
    u16 pixel_buffer[0x400];
    u16 *pixel_cursor;
    s32 band_offset;
    s32 pixel_index;
    s32 destination_base_y;

    if ((u32)side_index < 2) {
        destination_base_y = (side_index << 4) + 0xD0;

        if (tint_control >= COLOR_TINT_KEEP_HUE &&
            (tint_control & COLOR_TINT_INVERT) == 0 && saturation_scale >= COLOR_FIXED_ONE) {
            goto big;
        }

        band_offset = 0;
        gModel_sTextureTintRectX = 0x280;
        gModel_sTextureTintRectY = destination_base_y + 8;
        gModel_sTextureTintRectW = 0x100;
        gModel_sTextureTintRectH = 4;

        do {
            {
                s32 source_row_offset = band_offset + 8;

                gModel_sTextureTintRectY = destination_base_y + source_row_offset;
            }
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }

            pixel_cursor = pixel_buffer;
            for (pixel_index = 0; pixel_index < 0x400; pixel_index++, pixel_cursor++) {
                *pixel_cursor = Color_TintBgr555Pixel(*pixel_cursor, (u8)tint_control, (u16)saturation_scale);
            }

            gModel_sTextureTintRectY = destination_base_y + band_offset;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            band_offset += 4;
        } while (band_offset < 8);
    }

    return;

big:
    gModel_sTextureTintRectX = 0x280;
    gModel_sTextureTintRectY = destination_base_y + 8;
    gModel_sTextureTintRectW = 0x100;
    gModel_sTextureTintRectH = 8;
    MoveImage(TEXTURE_TINT_RECT, 0x280, destination_base_y);
}
#else
void Model_ApplyTextureTint(s32 side_index, s32 tint_control, s32 saturation_scale)
{
    u16 pixel_buffer[0x400];
    u16 *pixel_cursor;
    s32 band_offset;
    s32 pixel_index;

    if ((u32)side_index < 2) {
        band_offset = 0;

        if (tint_control >= COLOR_TINT_KEEP_HUE &&
            (tint_control & COLOR_TINT_INVERT) == 0 && saturation_scale >= COLOR_FIXED_ONE) {
            goto big;
        }

        gModel_sTextureTintRectX = side_index << 8;
        gModel_sTextureTintRectY = 0xF8;
        gModel_sTextureTintRectW = 0x100;
        gModel_sTextureTintRectH = 4;

        do {
            gModel_sTextureTintRectY = band_offset + 0xF8;
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }

            pixel_cursor = pixel_buffer;
            for (pixel_index = 0; pixel_index < 0x400; pixel_index++, pixel_cursor++) {
                *pixel_cursor = Color_TintBgr555Pixel(*pixel_cursor, (u8)tint_control, (u16)saturation_scale);
            }

            gModel_sTextureTintRectY = band_offset + 0xF0;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            band_offset += 4;
        } while (band_offset < 8);
    }

    return;

big:
    gModel_sTextureTintRectY = 0xF8;
    gModel_sTextureTintRectW = 0x100;
    gModel_sTextureTintRectX = side_index << 8;
    gModel_sTextureTintRectH = 8;
    MoveImage(TEXTURE_TINT_RECT, side_index << 8, 0xF0);
}
#endif

/* &gModel_RView.vrx. Keep the interior symbol so the object retains the retail
 * relocation target while the canonical GsRVIEW2 owns the surrounding view. */

void Model_StepViewOrbit(s32 direction_scale, s32 yaw_delta, s32 pitch_delta, s32 distance_delta)
{
    PSXLONG *pivot_position;
    PSXLONG *orbit_position;
    s32 cosine;
    s32 sine;
    s32 distance;
    s32 horizontal_or_x;
    s32 vertical_offset;
    s32 z_offset;
    s32 angle_sum;

    /* pivot_position and orbit_position are the two long triples at the head of the reference view,
       vpx..vpz and vrx..vrz: orbit_position is moved around pivot_position. */
    if (direction_scale > 0) {
        pivot_position = (PSXLONG *)gModel_ViewReferencePoint;
        orbit_position = pivot_position - 3;
    } else {
        pivot_position = &gModel_RView.vpx;
        orbit_position = pivot_position + 3;
    }

    if (*(s16 *)&gModel_wViewDistance + distance_delta >= 0xB) {
        *(u16 *)&gModel_wViewDistance = *(u16 *)&gModel_wViewDistance + distance_delta;
    }

    if ((yaw_delta & MODEL_ANGLE_MASK) != 0) {
        angle_sum = *(s16 *)&gModel_wViewYaw + yaw_delta + MODEL_ANGLE_FULL_TURN;
        gModel_wViewYaw = angle_sum - angle_sum / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if ((pitch_delta & MODEL_ANGLE_MASK) != 0) {
        angle_sum = *(s16 *)&gModel_wViewPitch + pitch_delta + MODEL_ANGLE_FULL_TURN;
        gModel_wViewPitch = angle_sum - angle_sum / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if (yaw_delta != 0 || pitch_delta != 0 || distance_delta != 0) {
        cosine = rcos(*(s16 *)&gModel_wViewPitch);
        sine = rsin(*(s16 *)&gModel_wViewPitch);
        distance = *(s16 *)&gModel_wViewDistance;
        horizontal_or_x = distance * cosine / MODEL_FIXED_ONE;
        vertical_offset = distance * sine / MODEL_FIXED_ONE;
        cosine = rcos(*(s16 *)&gModel_wViewYaw);
        sine = rsin(*(s16 *)&gModel_wViewYaw);
        z_offset = horizontal_or_x * sine / MODEL_FIXED_ONE;
        horizontal_or_x = horizontal_or_x * cosine / MODEL_FIXED_ONE;
        orbit_position[0] = pivot_position[0] + horizontal_or_x * direction_scale;
        orbit_position[1] = pivot_position[1] + vertical_offset * direction_scale;
        orbit_position[2] = pivot_position[2] + z_offset * direction_scale;
    }
}
