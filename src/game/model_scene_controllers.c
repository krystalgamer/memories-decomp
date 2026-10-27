/*
 * The model scene runtime between the background renderer and the debug
 * controller: the background VRAM animation step, the four scene-mode
 * controllers (animated battle, Exodia, Model_UpdateCreditsMonsterSlot, credits), the
 * Model_StartBattleSlotAction/Model_PlaySlotAnimationSounds pair, and the camera/view correction, tint,
 * camera-move setup and scene reset/configuration functions.
 *
 * Every feature switch below selects a header view for the whole unit, so it
 * comes before the first include. MODEL_HANDLER_* and
 * MODEL_GRAPHICS_STATE_SCENE_BYTES select retail's split absolute addresses
 * and scene-byte views for the intro controllers;
 * FUNC_80058434_CALL_WITH_UNUSED_ARG and D_8009B_MODEL_VISIBLE are the scene
 * setup functions' views.
 */
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#define MODEL_HANDLER_VIEW_OFFSET_ABSOLUTE
#define MODEL_HANDLER_OFFSET_ABSOLUTE
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#define D_8009B_MODEL_VISIBLE

#include "../types.h"
#include "../psyq/libetc.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "model_image_copy_frames.h"
#include "model_graphics_state.h"
#include "model_effect_requests.h"
#include "model_transfer_flags.h"
#include "sound_output_state.h"
#include "../psyq/stdio.h"
#include "model.h"
#include "model_slot_control.h"
#include "model_get_frame_step.h"
#include "sound.h"
#include "sd_calc_spatial_volume_pan.h"
#include "model_view_adjustments.h"
#include "model_has_insufficient_buffer_space.h"
#include "model_update_background_scroll.h"
#include "model_set_slot_shadow_enabled.h"
#include "model_effect_coefficients.h"
#include "model_slot_part_animation.h"
#include "model_separate_view.h"
#include "model_update_tint_requests.h"
#include "model_start_camera_move.h"
#include "model_record_tables.h"
#include "model_background.h"
#include "model_state_setters.h"
#include "model_update_view_metrics.h"
#include "model_scene_controllers.h"
#include "model_scene_effect_handler_table.h"
#include "model_handler_state.h"
#include "duel_card.h"
#include "model_slot_properties.h"
#include "sound_output.h"
#include "model_effect_state.h"
#include "model_lock_orbit_direction.h"
#include "model_is_camera_move_complete.h"
#include "model_effect_wrapped_value.h"
#include "model_transfer_state.h"
#include "model_distance_queries.h"
#include "model_scene_states.h"
#include "fade.h"
#include "sound_pending_entries.h"
#include "model_load_step.h"
#include "model_slot_state_updates.h"
#include "model_queue_relative_camera_orbit.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "high_memory_addresses.h"
#include "model_load_words.h"
#include "../psyq/libgs.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h, all included above. */
#include "../psyq/libhmd.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "model_update_coordinate_unit_matrix.h"
#include "model_cleanup.h"
#include "model_scene_imports.h"
#include "file_names.h"
#include "sound_init.h"
#include "model_step_view_orbit.h"
#include "camera_view.h"
#include "../psyq/memory.h"
#include "../ygo_types.h"
#include "model_draw_slot.h"
#include "../psyq/stdarg.h"
#include "../unmatched.h"
#include "fade_state_bytes.h"
#include "model_camera_state.h"
#include "graphics_frame_controls.h"

void Model_StepBackgroundVramAnimation(void) {
    s32 k;
    s32 u_0;
    s32 u_1;
    s32 r;
    s32 r4;
    s32 n;
    s32 q;

    if (gModel_bAuxiliaryActive[0] != 0) {
        k = gModel_pBackgroundRecord->selector;
        switch (k) {
        case 1:
            u_0 = VSync(-1) / 8;
            gModel_ImageCopyRectY = 0x1C0;
            gModel_ImageCopyRectW = 0x10;
            gModel_ImageCopyRectH = 0x40;
            r = u_0 % 3;
            gModel_ImageCopyRect.x = r * 16 + 0x2D0;
            MoveImage(&gModel_ImageCopyRect, 0x2C0, 0x1C0);
            break;
        case 4:
            n = MODEL_IMAGE_COPY_FRAME_COUNT;
            u_1 = VSync(-1) / 10;
            r4 = u_1 % n;
            gModel_ImageCopyRect.x = 0x260;
            q = gModel_abImageCopyFrameRows[r4];
            gModel_ImageCopyRectW = 0x20;
            gModel_ImageCopyRectH = 0x18;
            gModel_ImageCopyRectY = q * 3 * 8 + 0x180;
            MoveImage(&gModel_ImageCopyRect, 0x220, 0x168);
            break;
        }
    }
}

/*
 * Model scene controllers: the mode 15 controller, then the mode 19 battle
 * intro and its two neighbours.
 *
 * Model_UpdateAnimatedBattle, the mode 15 controller, copies the scene's four
 * effect handlers and its eight-byte view offset, then steps the signed
 * gModel_bPresentationPhase phase through lighting, fades, effect requests and the per-slot
 * handler calls. It matches all 4,908 text bytes and owns the 43-entry phase
 * table and the 7-entry model-kind table that precede the intro controller's
 * tables. Each phase keeps its temporaries in its own block, which is what lets
 * retail reuse $s0 across phases; the angle-window tests keep a bound compare
 * in each signed arm, and phases 13 and 17 classify the animation state with
 * nested inequality tests rather than a switch.
 *
 * The view-offset words and the diagnostics string use their unknown-bound
 * declarations, which select retail's split absolute addresses; the
 * switches for them, and for the two intro-controller views, are at the top
 * of the unit.
 */

void Model_UpdateAnimatedBattle(void)
{
    s32 (*effect_handlers[4])(s32, s32);
    SVECTOR camera_endpoint;
    s32 defender_handler_index;

    *(ModelSlotS32Quad *)effect_handlers = *(ModelSlotS32Quad *)gModel_apfnEffectHandlers;
    defender_handler_index = 1;
    if (((gDuel_adwCardStats[gModel_aSlots[1].model_id] >> 0x1A) & 0x1F) == 1) {
        defender_handler_index = 2;
    }
    switch ((s8)(gModel_bPresentationPhase + 1)) {
    case 0:
        break;
    case 1:
        Model_SetSlotAmbient(2, -0x1000, -0x1000, -0x1000);
        Model_SetSlotTintTarget(0, 5, 0, 0, 0);
        Model_SetSlotTintTarget(1, 5, 0, 0, 0);
        gModel_bPresentationPhase++;
        break;
    case 2:
        SD_BGMPlayDefaultStep(gModel_wPresentationBgmCommand);
        gModel_bPresentationPhase++;
        break;
    case 3: {
        ModelCameraOrbitPreset *preset;
        s32 key_magnitude;

        *(ModelBytes8 *)&camera_endpoint = *(ModelBytes8 *)gModel_AnimatedBattleCameraEndpoint;
        key_magnitude = 0x78;
        switch (gModel_pBackgroundRecord->selector) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            Model_SetCameraOrbitFromPreset(2, 0xA, 0);
            break;
        case 2:
            preset = ModelEffect_GetCoefficient(0xA);
            Model_SetCameraOrbit(2, 0x64, preset->yaw + 0x800, -0x400, 0);
            camera_endpoint.vx = preset->distance;
            camera_endpoint.vy = -0x1000;
            camera_endpoint.vz = preset->pitch + 0x400;
            key_magnitude = 0x96;
            break;
        }
        Model_ApplyEffectKeyQueueCommand(0, (void *)&camera_endpoint, (void *)0, key_magnitude);
        gModel_bPresentationPhase++;
        break;
    }
    case 4:
        if (gModel_aSlots[2].ambient_rgb_scale[0] < 0x800) {
            Model_SetSlotDrawMode(2, 3);
            gModel_aSlots[2].ambient_rgb_scale[0] += 0x40;
            gModel_aSlots[2].ambient_rgb_scale[1] += 0x40;
            gModel_aSlots[2].ambient_rgb_scale[2] += 0x40;
        } else if (Model_IsEffectKeyQueueEmpty() != 0) {
            Model_RequestCameraMove(0);
            Model_SetEffectKeyBatchingEnabled(1);
            Model_QueueCameraOrbitFromPreset(0, 0, 0, -0xC);
            Model_QueueEffectEndpointPair(0, 0, 0x4000);
            gModel_bPresentationPhase++;
        }
        break;
    case 5:
        if (gModel_aSlots[0].is_active != 0) {
            s16 *current_key_halfwords = (s16 *)Model_GetCurrentEffectKeyAddress();
            if (current_key_halfwords != 0 && current_key_halfwords[3] == 2) {
                Model_SetEffectKeyBatchingEnabled(1);
                Model_QueueCameraOrbitFromPreset(0, 0, 0, -0xC);
                Model_QueueEffectEndpointPair(0, 0, 0x4000);
                gModel_bActiveSlotIndex = 0;
                effect_handlers[0](gModel_aSlots[0].variant_module_data_address, 0);
                gModel_aSlots[0].field_E15 = 0;
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 6:
        if (gModel_aSlots[0].field_E15 == 0) {
            gModel_bActiveSlotIndex = 0;
            if (effect_handlers[0](gModel_aSlots[0].variant_module_data_address, -1) != 2) {
                break;
            }
            gModel_aSlots[0].field_E15++;
        }
        if ((s8)gModel_aSlots[0].tint[0] >= 0) {
            gModel_aSlots[0].tint[0] += 2;
            gModel_aSlots[0].tint[1] += 2;
            gModel_aSlots[0].tint[2] += 2;
        } else {
            if (gModel_aSlots[0].draw_mode_request != 4) {
                Model_SetSlotDrawMode(0, 3);
            }
            Model_SetSlotTintTarget(0, 0, 0x80, 0x80, 0x80);
            Model_SetSlotTintTarget(1, 0, 0x80, 0x80, 0x80);
            Model_ControlSlotAnimation(0, 2, 1);
            gModel_bPresentationTimer = 0;
            gModel_bPresentationPhase++;
        }
        break;
    case 7:
        if (gModel_aSlots[0].draw_mode_request != 4) {
            Model_SetSlotDrawMode(0, 3);
        }
        if (gModel_aSlots[0].active_animation_command_row == 0) {
            gModel_bPresentationTimer += Model_GetFrameStep();
            if (gModel_bPresentationTimer >= 0x3C && gModel_aSlots[1].is_active != 0) {
                Model_SetEffectKeyBatchingEnabled(1);
                Model_QueueCameraOrbitFromPreset(1, 0, 0, -0xA);
                Model_QueueEffectEndpointPair(1, 1, 0x4000);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 8: {
        s16 *current_key_halfwords;

        if (gModel_aSlots[0].draw_mode_request != 4) {
            Model_SetSlotDrawMode(0, 3);
        }
        current_key_halfwords = (s16 *)Model_GetCurrentEffectKeyAddress();
        if (current_key_halfwords != 0 && current_key_halfwords[3] == 2) {
            Model_SetCameraOrbitFromPreset(1, 0, 0);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 9:
        if ((s8)gModel_aSlots[1].tint[0] >= 0) {
            gModel_aSlots[1].tint[0] += 2;
            gModel_aSlots[1].tint[1] += 2;
            gModel_aSlots[1].tint[2] += 2;
        } else {
            Model_ControlSlotAnimation(1, 2, 1);
            gModel_bPresentationTimer = 0;
            gModel_bPresentationPhase++;
        }
        break;
    case 10:
        if (gModel_aSlots[1].active_animation_command_row == 0) {
            gModel_bPresentationTimer += Model_GetFrameStep();
            if (gModel_bPresentationTimer >= 0x3C) {
                s32 preset_index;
                s32 key_magnitude;

                preset_index = gModel_aSlots[0].metadata.prefix.bytes.guardian_star_options[gModel_aSlots[0].guardian_star_selection] & 0x1F;
                key_magnitude = -0x14;
                if (preset_index == 1) {
                    goto s10_m0;
                }
                if (preset_index < 2) {
                    goto s10_m3;
                }
                if (preset_index == 2) {
                    goto s10_m1;
                }
                if (preset_index == 3) {
                    goto s10_m2;
                }
                preset_index = 3;
                goto s10_teste;
            s10_m0:
                preset_index = 0;
                goto s10_teste;
            s10_m1:
                preset_index = 1;
                goto s10_teste;
            s10_m2:
                preset_index = 2;
                goto s10_teste;
            s10_m3:
                preset_index = 3;
            s10_teste:
                if ((u32)preset_index < 2) {
                    key_magnitude = key_magnitude / 2;
                }
                Model_QueueCameraOrbitFromPreset(0, preset_index, 0, key_magnitude);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 11:
        if (Model_IsEffectKeyQueueEmpty() != 0) {
            Model_RequestCameraMoveToSlots(1, -1, 0, -0x1E);
            gModel_bPresentationPhase++;
        }
        break;
    case 12:
        if (Model_IsCameraMoveComplete(1) != 0) {
            s32 target_yaw;
            s32 yaw_step_limit;
            s32 yaw_delta;

            target_yaw = Model_GetSlotCameraPresetYaw(0);
            yaw_step_limit = (Model_GetFrameStep() + 2) * 8;
            yaw_delta = (s16)gModel_wViewYaw - target_yaw;
            if (yaw_delta >= 0 ? yaw_step_limit >= yaw_delta : yaw_step_limit >= target_yaw - (s16)gModel_wViewYaw) {
                Model_StartBattleSlotAction(0);
                gModel_bActiveSlotIndex = 1;
                effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, 0x18E);
                gModel_aSlots[1].field_E15 = 0xFF;
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 13: {
        s32 a;
        s32 move_magnitude;

        if (gModel_aSlots[1].active_animation_command_row != 5) {
            if (gModel_aSlots[1].active_animation_command_row != 6) {
                if (gModel_aSlots[1].active_animation_command_row == 8) {
                    a = 3;
                } else {
                    a = 0;
                }
            } else {
                a = 2;
            }
        } else {
            a = 1;
        }
        if (gModel_aSlots[1].field_E15 == 0xFF && gModel_aSlots[1].active_animation_command_row == 8) {
            gModel_aSlots[1].field_E15 = 0;
        }
        if (gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, -1);
        }
        if (gModel_aSlots[0].animation_control_state == 2) {
            if (gModel_aSlots[0].active_animation_command_row == 0) {
                a = 4;
            }
        } else if (Model_GetCameraHoldState() == 0) {
            a = 0;
        }
        if (a != 0) {
            a = Model_GetSlotDistanceFromCameraTarget(1) * 0x1E / 1000;
            if (Model_GetCameraHoldState() == 2) {
                Model_SetCameraHoldState(1);
            }
            Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
            move_magnitude = a;
            if (move_magnitude < 0xA) {
                move_magnitude = 0xA;
            }
            Model_RequestCameraMoveToSlots(1, -1, 1, move_magnitude);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 14:
        if (gModel_aSlots[1].active_animation_command_row == 6) {
            gModel_bPresentationPhase = 0x14;
            break;
        }
        if (gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, -1);
        }
        if (Model_IsCameraMoveComplete(1) != 0 && Model_IsEffectKeyQueueEmpty() != 0
            && gModel_aSlots[0].animation_control_state == 2 && gModel_aSlots[0].active_animation_command_row == 0
            && gModel_aSlots[1].animation_control_state == 2 && gModel_aSlots[1].active_animation_command_row == 0) {
            if (gModel_aSlots[1].defense_position != 0) {
                Model_SetCameraFollowCountdown(0x78);
                gModel_bPresentationPhase = 0x1E;
            } else {
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 15:
        if (Model_IsEffectKeyQueueEmpty() != 0) {
            Model_RequestCameraMoveToSlots(1, -1, 1, -0x1E);
            gModel_bPresentationPhase++;
        }
        break;
    case 16:
        if (Model_IsCameraMoveComplete(1) != 0) {
            s32 target_yaw;
            s32 yaw_step_limit;
            s32 yaw_delta;

            target_yaw = Model_GetSlotCameraPresetYaw(1);
            yaw_step_limit = (Model_GetFrameStep() + 2) * 8;
            yaw_delta = (s16)gModel_wViewYaw - target_yaw;
            if (yaw_delta >= 0 ? yaw_step_limit >= yaw_delta : yaw_step_limit >= target_yaw - (s16)gModel_wViewYaw) {
                Model_StartBattleSlotAction(1);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 17: {
        s32 a;
        s32 move_magnitude;

        if (gModel_aSlots[0].active_animation_command_row != 5) {
            if (gModel_aSlots[0].active_animation_command_row != 6) {
                if (gModel_aSlots[0].active_animation_command_row == 8) {
                    a = 3;
                } else {
                    a = 0;
                }
            } else {
                a = 2;
            }
        } else {
            a = 1;
        }
        if (gModel_aSlots[1].animation_control_state == 2) {
            if (gModel_aSlots[1].active_animation_command_row == 0) {
                a = 4;
            }
        } else if (Model_GetCameraHoldState() == 0) {
            a = 0;
        }
        if (a != 0) {
            a = Model_GetSlotDistanceFromCameraTarget(0) * 0x1E / 1000;
            if (Model_GetCameraHoldState() == 2) {
                Model_SetCameraHoldState(1);
            }
            Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
            move_magnitude = a;
            if (move_magnitude < 0xA) {
                move_magnitude = 0xA;
            }
            Model_RequestCameraMoveToSlots(1, -1, 0, move_magnitude);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 18:
        if (gModel_aSlots[0].active_animation_command_row == 6) {
            gModel_bPresentationPhase = 0x14;
            break;
        }
#ifdef VERSION_JAPAN
        /* The Japanese build polls the same two calls but prints nothing. */
        if (Model_IsCameraMoveComplete(1) != 0) {
            Model_IsEffectKeyQueueEmpty();
        }
#else
        if (Model_IsCameraMoveComplete(1) != 0 && Model_IsEffectKeyQueueEmpty() != 0
            && gModel_aSlots[0].animation_control_state == 2 && gModel_aSlots[0].active_animation_command_row == 0
            && gModel_aSlots[1].animation_control_state == 2 && gModel_aSlots[1].active_animation_command_row == 0) {
            FntPrint(gModel_szWhyTrace);
        }
#endif
        break;
    case 21: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row != 6;
        ModelSlot *opposite_slot = &gModel_aSlots[selected_slot_index ^ 1];

        if (opposite_slot->animation_control_state == 2) {
            if (opposite_slot->active_animation_command_row == 0 && gModel_aSlots[selected_slot_index].playback_state == 0x23) {
                if (gModel_aSlots[1].defense_position == 0
                    && gModel_aSlots[0].metadata.prefix.values.attack_compare_value == gModel_aSlots[1].metadata.prefix.values.attack_compare_value) {
                    if (gModel_aSlots[1].stone_texture_state == 0xFF) {
                        gModel_aSlots[1].stone_texture_state = 0x80;
                    }
                    Model_ControlSlotAnimation(1, 6, 1);
                }
                Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
                if (Model_IsCameraMoveComplete(1) != 0) {
                    s32 move_magnitude = Model_GetSlotDistanceFromCameraTarget(selected_slot_index) * 0x1E / 1000;
                    if (move_magnitude < 0xA) {
                        move_magnitude = 0xA;
                    }
                    Model_RequestCameraMoveToSlots(1, -1, selected_slot_index, -move_magnitude);
                }
                gModel_bPresentationPhase++;
            }
        }
        break;
    }
    case 22: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row != 6;
        ModelSlot *selected_slot = &gModel_aSlots[selected_slot_index];

        if (selected_slot->stone_texture_state == 0 && Model_IsCameraMoveComplete(1) != 0) {
            Model_SetSlotTintTarget(selected_slot_index, 5, 0x80, 0x80, 0x80);
            gModel_bActiveSlotIndex = selected_slot_index;
            effect_handlers[3](selected_slot->variant_module_data_address, 0);
            selected_slot->field_E15 = 0;
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 23: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row != 6;
        ModelSlot *selected_slot = &gModel_aSlots[selected_slot_index];
        s32 biased_yaw;
        s32 current_yaw;

        if (selected_slot->field_E15 == 0) {
            gModel_bActiveSlotIndex = selected_slot_index;
            if (effect_handlers[3](selected_slot->variant_module_data_address, -1) != 2) {
                break;
            }
            selected_slot->field_E15++;
        }
        if (selected_slot->tint[0] != 0) {
            selected_slot->tint[0] -= 2;
            selected_slot->tint[1] -= 2;
            selected_slot->tint[2] -= 2;
            break;
        }
        current_yaw = (s16)gModel_wViewYaw;
        if (selected_slot_index > 0) {
            biased_yaw = (current_yaw + 0xC00) % 0x1000;
        } else {
            biased_yaw = (current_yaw + 0x400) % 0x1000;
        }
        if ((u32)(biased_yaw - 0x201) < 0xBFF) {
            s32 opposite_slot_index = selected_slot_index ^ 1;
            s32 move_magnitude = Model_GetSlotDistanceFromCameraTarget(opposite_slot_index) * 0x1E / 1000;

            if (move_magnitude < 0x1E) {
                move_magnitude = 0x1E;
            }
            Model_RequestCameraMoveToSlots(1, -1, opposite_slot_index, -move_magnitude);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 24: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;

        if (Model_IsEffectKeyQueueEmpty() != 0) {
            s32 current_distance = (s16)gModel_wViewDistance;
            if (Model_AdjustCameraDistanceForSlot(selected_slot_index, 0x2BC) + 0xA >= current_distance && Model_IsCameraMoveComplete(1) != 0) {
                if (gModel_aSlots[0].metadata.prefix.values.attack_compare_value == gModel_aSlots[1].metadata.prefix.values.attack_compare_value
                    && gModel_aSlots[1].defense_position == 0) {
                    gModel_bActiveSlotIndex = 1;
                    effect_handlers[3](gModel_aSlots[1].variant_module_data_address, 0);
                    gModel_aSlots[1].field_E15 = 0;
                    gModel_bPresentationPhase = 0x19;
                    gModel_bPresentationTimer = 0;
                } else {
                    Model_QueueEffectEndpointPair(-1, selected_slot_index, 0x4000);
                    Model_SetCameraTargetSlots(-1, selected_slot_index);
                    Model_ControlSlotAnimation(selected_slot_index, 7, 1);
                    gModel_bPresentationPhase++;
                }
            }
        }
        break;
    }
    case 25: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;

        if (gModel_aSlots[selected_slot_index].active_animation_command_row != 7) {
            Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
            Model_RequestCameraMoveToSlots(1, -1, selected_slot_index, -0x1E);
            gModel_bPresentationTimer = 0x3C;
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 26: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;

        if (gModel_aSlots[selected_slot_index].stone_texture_state == 0 && Model_IsCameraMoveComplete(1) != 0) {
            gModel_bPresentationTimer += Model_GetFrameStep();
            if (gModel_bPresentationTimer >= 0x3C) {
                Model_SetSlotTintTarget(selected_slot_index, 5, 0x80, 0x80, 0x80);
                gModel_bPresentationPhase++;
            }
        }
        break;
    }
    case 27: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;
        ModelSlot *selected_slot;
        ModelSlot *slot_base;

        if (gModel_aSlots[0].metadata.prefix.values.attack_compare_value == gModel_aSlots[1].metadata.prefix.values.attack_compare_value
            && gModel_aSlots[1].defense_position == 0 && gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            if (effect_handlers[3](gModel_aSlots[1].variant_module_data_address, -1) != 2) {
                break;
            }
            gModel_aSlots[1].field_E15++;
        }
        slot_base = gModel_aSlots;
        selected_slot = &slot_base[selected_slot_index];
        if (selected_slot->tint[0] != 0) {
            selected_slot->tint[0] -= 2;
            selected_slot->tint[1] -= 2;
            selected_slot->tint[2] -= 2;
        } else {
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 28:
        if (Model_IsCameraMoveComplete(1) != 0) {
            gModel_bPresentationPhase = 0x28;
        }
        break;
    case 31:
        if (gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            if (effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, -2) == 2) {
                gModel_aSlots[1].field_E15++;
            }
        } else if (Model_IsCameraMoveComplete(1) != 0) {
            Model_SetSlotTintTarget(1, 5, 0x80, 0x80, 0x80);
            gModel_bPresentationPhase++;
        }
        break;
    case 32:
        if (gModel_aSlots[1].tint[0] != 0) {
            gModel_aSlots[1].tint[0] -= 2;
            gModel_aSlots[1].tint[1] -= 2;
            gModel_aSlots[1].tint[2] -= 2;
        } else {
            s32 biased_yaw = (s16)gModel_wViewYaw + 0xC00;

            if ((u32)(biased_yaw % 0x1000 - 0x201) < 0xBFF) {
                s32 move_magnitude = Model_GetSlotDistanceFromCameraTarget(0) * 0x1E / 1000;
                if (move_magnitude < 0x1E) {
                    move_magnitude = 0x1E;
                }
                Model_RequestCameraMoveToSlots(1, -1, 0, -move_magnitude);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 33:
        if (Model_IsEffectKeyQueueEmpty() != 0 && Model_IsCameraMoveComplete(1) != 0) {
            Model_SetSlotTintTarget(0, 5, 0x80, 0x80, 0x80);
            gModel_bPresentationPhase++;
        }
        break;
    case 34:
        if (gModel_aSlots[0].tint[0] != 0) {
            gModel_aSlots[0].tint[0] -= 2;
            gModel_aSlots[0].tint[1] -= 2;
            gModel_aSlots[0].tint[2] -= 2;
        } else {
            gModel_bPresentationPhase = 0x28;
        }
        break;
    case 41:
        Model_SetSlotDrawMode(2, 3);
        Fade_StartOut();
        gModel_bPresentationPhase++;
        break;
    case 42:
        Model_SetSlotDrawMode(2, 3);
        if (!(gFade_bFlags[0] & 0x80)) {
            SD_ResetPendingEntries();
            gModel_bPresentationPhase = -2;
        }
        break;
    }
    Model_LockOrbitDirection(1);
}

/*
 * Scene mode 19's intro controller. The control-module entry points and
 * eight-byte view offset are captured before the frame-step setter; the
 * signed phase, module data arguments and model fields remain live.
 *
 * The full reconstruction matches all 1,880 text bytes and the 17-entry
 * signed-phase table with the uniform GCC 2.8.1 G8 split-address profile.
 * The byte block-move view preserves the original unaligned eight-byte copy.
 * The timing callback is sequenced before the final live scale-byte read.
 */

void Model_UpdateExodiaPresentation(void)
{
    SVECTOR camera_offset;
    ModelSlot *primary_slot;
    s32 (*slot0_module_handler)(s32, s32);
    s32 (*slot1_module_handler)(s32, s32);

    *(ModelBytes8 *)&camera_offset = *(ModelBytes8 *)gModel_ExodiaOrbitOffset;
    primary_slot = gModel_aSlots;
    slot0_module_handler = (s32 (*)(s32, s32))(gModel_Slot0VariantModuleAddress + 4);
    slot1_module_handler = (s32 (*)(s32, s32))(gModel_Slot1VariantModuleAddress + 4);
    Model_SetFrameStepOverride(1);
    if ((u32)((u8)gModel_bPresentationPhase - 8) < 2 && primary_slot->field_E15 == 0) {
        gModel_bActiveSlotIndex = 0;
        if (slot0_module_handler(gModel_Slot0VariantDataAddress, -1) == 2) {
            primary_slot->field_E15++;
        }
    }
    if (gModel_bPresentationPhase >= 10 && gModel_aSlots[1].field_E15 == 0) {
        gModel_bActiveSlotIndex = 0;
        if (slot1_module_handler(gModel_Slot1VariantDataAddress, -1) == 2) {
            Model_SetSlotPlaybackDirection(0, 0);
            Fade_CutToWhite();
            gModel_bPresentationPhase = 15;
        }
    }
    switch ((s8)(gModel_bPresentationPhase + 1)) {
    case 0: {
        ModelSlot *load_slots;
        if (gModel_aSlots[0].load_step == 0xFF) {
            if ((gFile_dwTransferState_abs & 0x2000030) | gFile_dwCancelRequestState_abs) {
                goto check_ready;
            }
            Model_LoadMonsterMerge(
                0, MODEL_SPECIAL_BATTLE_ID, 0, 0, 0, 0, 4);
        } else {
            Model_AdvanceSlotLoad(0);
        }
check_ready:
        load_slots = gModel_aSlots;
        if (!load_slots->is_active) {
            break;
        }
        Model_SetCameraOrbit(0, 10000, 0xE00, 0, 0);
        Model_SeekSlotAnimation(0, 1, 0);
        if (primary_slot->active_animation_command_row) {
            Model_ControlSlotAnimation(0, 0, 0);
        }
        Model_SetSlotPlaybackDirection(0, 1);
        Model_SetSlotTintTarget(0, 5, 0, 0, 0);
        primary_slot->queued_animation_row_latch = 1;
        primary_slot->queued_animation_row = 2;
        SD_BGMPlayDefaultStep(0x7310);
        load_slots->metadata.first_star_module_command = -1;
        load_slots->metadata.second_star_module_command = -1;
        load_slots->metadata.primary_module_command = -1;
        gModel_bActiveSlotIndex = 0;
        slot0_module_handler(gModel_Slot0VariantDataAddress, 0);
        slot1_module_handler(gModel_Slot1VariantDataAddress, 0);
        load_slots[0].field_E15 = 0;
        load_slots[1].field_E15 = 0;
        gModel_bPresentationPhase++;
        break;
    }
    case 1:
        if (SD_GetStatusFlags() & 0x80) {
            camera_offset.vy = -2000;
            Model_SeekSlotAnimation(0, 0, 10);
            Model_QueueRelativeCameraOrbit(0, 2000, 0x100, 0x100, &camera_offset, 110);
            gModel_bPresentationPhase++;
        }
        break;
    case 2: {
        ModelSlot *tint_slot = gModel_aSlots;
        if ((s8)tint_slot->tint[0] >= 0) {
            tint_slot->tint[0] += 2;
            tint_slot->tint[1] += 2;
            tint_slot->tint[2] += 2;
        } else {
            Model_SetSlotTintTarget(0, 0, 0x80, 0x80, 0x80);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 3:
        if (Model_IsEffectKeyQueueEmpty() && primary_slot->animation_position >= 0x780) {
            Model_SeekSlotAnimation(0, 0, 0);
            gModel_bPresentationPhase++;
        }
        break;
    case 4:
        if (primary_slot->animation_position >= 0x780) {
            Model_SeekSlotAnimation(0, 0, 0);
            gModel_bPresentationPhase++;
        }
        break;
    case 5:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = -2500;
            camera_offset.vy = -1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, -0x100, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 6:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = 2500;
            camera_offset.vy = -1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, 0x100, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 7:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = -500;
            camera_offset.vy = 1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, -0x200, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 8:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = 500;
            camera_offset.vy = 1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, 0x200, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 9:
        if (primary_slot->animation_position >= 0x1E0) {
            camera_offset.vy = -2000;
            Model_SetCameraOrbit(0, 3500, 0x40, 0x200, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 10:
        if (primary_slot->current_animation_row == 2) {
            gModel_bPresentationPhase++;
        }
        break;
    case 11:
        if (primary_slot->animation_position >= 0x8C0) {
            camera_offset.vy = -1500;
            Model_QueueRelativeCameraOrbit(0, 3500, 0, -0x80, &camera_offset, 40);
            gModel_bPresentationPhase++;
        }
        break;
    case 12:
        if (primary_slot->animation_position >= 0xE60) {
            camera_offset.vz = -1000;
            Model_SetCameraOrbit(0, 5000, 0x200, -0x100, &camera_offset);
            Model_SeekSlotAnimation(0, 0, 190);
            gModel_bPresentationPhase++;
        }
        break;
    case 13:
        if (primary_slot->animation_position >= 0xE60) {
            camera_offset.vz = -1000;
            Model_SetCameraOrbit(0, 5000, -0x200, -0x100, &camera_offset);
            Model_SeekSlotAnimation(0, 0, 190);
            gModel_bPresentationPhase++;
        }
        break;
    case 14:
        if (primary_slot->animation_position >= 0xE60) {
            camera_offset.vy = -1000;
            Model_SetCameraOrbit(0, 3000, 0, -0x80, &camera_offset);
            Model_SeekSlotAnimation(0, 0, 190);
            gModel_bPresentationPhase++;
        }
        break;
    case 15: {
        s32 stop_position = primary_slot->animation_duration_rows[primary_slot->current_animation_row].max << 4;
        s32 frame_step = Model_GetFrameStep();
        stop_position -= primary_slot->playback_speed * frame_step;
        if (primary_slot->animation_position >= stop_position) {
            Model_SetSlotPlaybackDirection(0, 0);
        }
        break;
    }
    case 16:
        gModel_bPresentationPhase = -2;
        break;
    }
}

void Model_UpdateCreditsMonsterSlot(s32 slot_index) {
    ModelSlot *initial_slot;
    ModelSlot *ready_base;
    ModelSlot *initial_base;
    ModelSlot *peer_slot;
    GsCOORDUNIT *peer_coordinate;
    ModelSlot *ready_slot;
    s32 model_id;
    s32 t;
    s32 yaw_work;
    s32 m1;

    initial_base = gModel_aSlots;
    initial_slot = initial_base + slot_index;
    if (initial_slot->is_active == 0) {
        if (initial_slot->load_step == 0xFF) {
            if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 gFile_dwCancelRequestState_abs) == 0) {
                do {
                    t = rand() >> 8;
                    model_id = t % CARD_COUNT;
                } while (model_id < 0 || model_id >= CARD_COUNT ||
                         (model_id >= MODEL_MRG_FIRST_GAP_START
                          && model_id < MODEL_MRG_FIRST_GAP_END) ||
                         (model_id >= MODEL_MRG_SECOND_GAP_START
                          && model_id < MODEL_MRG_SECOND_GAP_END) ||
                         model_id == MODEL_MRG_SINGLE_GAP_ID);
                m1 = -1;
                Model_LoadMonsterMerge(slot_index | 0x80, model_id, m1, m1, m1, m1, 0);
            }
        } else {
            Model_AdvanceSlotLoad(slot_index);
        }

        ready_base = gModel_aSlots;
        if ((ready_base + slot_index)->is_active != 0) {
            peer_slot = ready_base + (slot_index ^ 1);
            yaw_work = MODEL_ANGLE_QUARTER_TURN;
            if (peer_slot->is_active != 0) {
                peer_coordinate = peer_slot->root_coordinate;
                if (peer_coordinate != (GsCOORDUNIT *)0) {
                    t = peer_coordinate->rot.vy + MODEL_ANGLE_QUARTER_TURN;
                    yaw_work = t / MODEL_ANGLE_FULL_TURN;
                    yaw_work = t - yaw_work * MODEL_ANGLE_FULL_TURN;
                }
            }
            ready_slot = gModel_aSlots + slot_index;
            if (ready_slot->root_coordinate != (GsCOORDUNIT *)0) {
                ready_slot->root_coordinate->rot.vx = 0;
                ready_slot->root_coordinate->rot.vy = yaw_work;
                ready_slot->root_coordinate->rot.vz = 0;
                ready_slot->root_coordinate->matrix.t[0] = 0;
                ready_slot->root_coordinate->matrix.t[1] = 0;
                ready_slot->root_coordinate->matrix.t[2] = 0;
            }
            Model_UpdateCoordinateUnitMatrix(ready_slot->root_coordinate, 0);
            Model_RunSlotHandlers(slot_index);
            ready_slot->field_E15 = 0;
        }
    }
}

#define ACTIVE_SLOT gGraphics_FrameStepControl[3]
#define MODEL_SLOT_VIEW(slot) ((ModelSlot *)(slot))
#define MODEL_SLOT_RAW_BYTES(slot) ((u8 *)(slot))

#ifndef MODEL_INTRO_PACKAGE_START_SECTOR
#define MODEL_INTRO_PACKAGE_START_SECTOR 1223
#endif

void Model_UpdateCreditsPresentation(void)
{
  s8 phase_value;
  ModelSlot *load_check_base;
  ModelSlot *crossfade_base;
  ModelSlot *active_lookup_base;
  ModelSlot *fade_slot_base;
  u8 *slot0_tint;
  u8 *slot1_tint;
  s32 active_offset;
  u8 *active_color;
  s32 red_sample;
  s32 red_target;
  s32 green_sample;
  s32 green_target;
  s32 red;
  s32 blue_target;
  s8 next_red;
  s32 green;
  s8 next_green;
  s32 blue;
  s8 next_blue;
  s32 *active_slot_words;
  u32 slot_tick_count;
  s32 animation_row;
  s8 next_phase;
  s32 peer_slot_index;
  s32 crossfade_offset;
  u8 *outgoing_color;
  u8 *incoming_color;
  u32 peer_red;
  u32 peer_green;
  u32 peer_blue;
  s32 result;

  Model_SetFrameStepOverride(1);
  switch ( gModel_bPresentationPhase )
  {
    case -1:
      File_RequestAsyncTransfer(
          1, gModel_szSuMrgPath, MODEL_INTRO_PACKAGE_START_SECTOR,
          16, 0, 0, gModel_CreditsModuleAddress);
      File_WaitForTransfers();
      D_8009B004.fields.credits_exit_requested = 0;
      D_8009B004.fields.credits_update_enabled = 0;
      ACTIVE_SLOT = 0;
      gModel_aSlots[0].is_active = 0;
      gModel_aSlots[1].is_active = 0;
      Model_SetSlotTintTarget(0, 2, 0, 0, 0);
      Model_SetSlotTintTarget(1, 2, 0, 0, 0);
      func_801807B0();
      func_80181C4C(0);
      SD_SetMusicTrackBuffer((u16 *)gModel_pCreditsMusicTrackBuffer);
      SD_BGMPlayDefaultStep(29488);
      phase_value = gModel_bPresentationPhase + 1;
      goto store_phase;
    case 0:
      load_check_base = gModel_aSlots;
      active_offset = (u32)&((ModelSlot *)0)[(u8)ACTIVE_SLOT];
      if ( MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(load_check_base) + active_offset)->is_active )
      {
        if ( (MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + active_offset)->field_E15 & 3) == 0 )
        {
          active_color = MODEL_SLOT_VIEW(
              MODEL_SLOT_RAW_BYTES(gModel_aSlots) + active_offset)->tint;
          red_sample = rand() >> 8;
          red_sample %= 24;
          red_target = red_sample + 8;
          green_sample = rand() >> 8;
          green_sample %= 24;
          green_target = green_sample + 8;
          blue_target = rand() >> 8;
          blue_target %= 24;
          blue_target += 8;
          red = active_color[0];
          if ( red != red_target )
          {
            if ( red >= red_target )
              next_red = red - 1;
            else
              next_red = red + 1;
            active_color[0] = next_red;
          }
          green = active_color[1];
          if ( green != green_target )
          {
            if ( green >= green_target )
              next_green = green - 1;
            else
              next_green = green + 1;
            active_color[1] = next_green;
          }
          blue = active_color[2];
          if ( blue != blue_target )
          {
            if ( blue >= blue_target )
              next_blue = blue - 1;
            else
              next_blue = blue + 1;
            active_color[2] = next_blue;
          }
        }
        if ( !D_8009B004.fields.credits_exit_requested )
          Model_UpdateCreditsMonsterSlot((u8)ACTIVE_SLOT ^ 1);
        active_lookup_base = gModel_aSlots;
        active_slot_words = (s32 *)&active_lookup_base[(u8)ACTIVE_SLOT];
        if ( !MODEL_SLOT_VIEW(active_slot_words)->active_animation_command_row )
        {
          slot_tick_count = MODEL_SLOT_VIEW(active_slot_words)->field_E15;
          animation_row = -1;
          if ( !(u8)(slot_tick_count % 0x1E) )
          {
            switch ( (u8)(slot_tick_count / 0x1E) )
            {
              case 1u:
                animation_row = 2;
                break;
              case 3u:
                animation_row = 7;
                break;
              case 0u:
              case 2u:
              case 4u:
                animation_row = 1;
                break;
              default:
                break;
            }
            if ( animation_row < 0 )
            {
              if ( D_8009B004.fields.credits_exit_requested )
              {
                next_phase = 2;
                goto set_phase_and_tick;
              }
              peer_slot_index = (u8)ACTIVE_SLOT ^ 1;
              if ( MODEL_SLOT_VIEW(
                       MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
                       (904 * peer_slot_index) * 4)->is_active )
              {
                Model_RequestCameraMoveToSlots(1, -1, peer_slot_index, 90);
                next_phase = gModel_bPresentationPhase + 1;
                goto set_phase_and_tick;
              }
            }
            else
            {
              Model_ControlSlotAnimation((u8)ACTIVE_SLOT, animation_row, 1);
            }
          }
          goto tick_active_slot;
        }
      }
      else
      {
        Model_UpdateCreditsMonsterSlot((u8)ACTIVE_SLOT);
        if ( MODEL_SLOT_VIEW(
                 MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
                 (904 * (u8)ACTIVE_SLOT) * 4)->is_active )
          Model_RequestCameraMoveToSlots(1, -1, (u8)ACTIVE_SLOT, 30);
      }
      goto poll_module;
    case 1:
      crossfade_base = gModel_aSlots;
      crossfade_offset = 904 * (u8)ACTIVE_SLOT;
      if ( (MODEL_SLOT_VIEW(
                MODEL_SLOT_RAW_BYTES(crossfade_base) +
                (crossfade_offset) * 4)->field_E15 & 3) == 0 )
      {
        outgoing_color = MODEL_SLOT_VIEW(
            MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
            (crossfade_offset) * 4)->tint;
        incoming_color = MODEL_SLOT_VIEW(
            MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
            (904 * ((u8)ACTIVE_SLOT ^ 1)) * 4)->tint;
        if ( outgoing_color[0] )
          --outgoing_color[0];
        if (outgoing_color[1])
          --outgoing_color[1];
        if (outgoing_color[2])
          --outgoing_color[2];
        peer_red = incoming_color[0];
        if ( peer_red < 8 )
          incoming_color[0] = peer_red + 1;
        peer_green = incoming_color[1];
        if ( peer_green < 8 )
          incoming_color[1] = peer_green + 1;
        peer_blue = incoming_color[2];
        if ( peer_blue < 8 )
          incoming_color[2] = peer_blue + 1;
        if ( !outgoing_color[0] && !outgoing_color[1] && !outgoing_color[2] )
        {
          MODEL_SLOT_VIEW(
              MODEL_SLOT_RAW_BYTES(crossfade_base) +
              (904 * (u8)ACTIVE_SLOT) * 4)->is_active = 0;
          gModel_bPresentationPhase = 0;
          ACTIVE_SLOT ^= 1u;
        }
      }
      goto tick_active_slot;
    case 2:
      fade_slot_base = gModel_aSlots;
      if ( (MODEL_SLOT_VIEW(
                MODEL_SLOT_RAW_BYTES(fade_slot_base) +
                (904 * (u8)ACTIVE_SLOT) * 4)->field_E15 & 3) != 0 )
        goto tick_active_slot;
      slot0_tint = fade_slot_base[0].tint;
      slot1_tint = fade_slot_base[1].tint;
      if ( fade_slot_base[0].tint[0] )
        --fade_slot_base[0].tint[0];
      if ( slot0_tint[1] )
        --slot0_tint[1];
      if ( slot0_tint[2] )
        --slot0_tint[2];
      if ( fade_slot_base[1].tint[0] )
        fade_slot_base[1].tint[0] = fade_slot_base[1].tint[0] - 1;
      if ( slot1_tint[1] )
        --slot1_tint[1];
      if ( slot1_tint[2] )
        --slot1_tint[2];
      if ( fade_slot_base[0].tint[0]
        || slot0_tint[1] || slot0_tint[2]
        || fade_slot_base[1].tint[0]
        || slot1_tint[1] || slot1_tint[2] )
      {
        goto tick_active_slot;
      }
      fade_slot_base[1].is_active = 0;
      fade_slot_base[0].is_active = 0;
      next_phase = gModel_bPresentationPhase + 1;
      goto set_phase_and_tick;
set_phase_and_tick:
      gModel_bPresentationPhase = next_phase;
tick_active_slot:
      ++MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + (904 * (u8)ACTIVE_SLOT) * 4)->field_E15;
      goto poll_module;
    case 3:
      SD_ResetMusicTrackBuffer();
      SD_ResetPendingEntries();
      phase_value = -2;
store_phase:
      gModel_bPresentationPhase = phase_value;
      goto poll_module;
    default:
      goto poll_module;
  }
poll_module:
      if ( D_8009B004.fields.credits_update_enabled )
      {
        result = func_80180A24();
        if ( result )
        {
          result = D_8009B004.fields.credits_exit_requested;
          if ( !D_8009B004.fields.credits_exit_requested )
          {
            result = (s16)SD_GetStatusFlags();
            if ( (s16)result != 128 )
            {
              ++D_8009B004.fields.credits_exit_requested;
              gModel_bPresentationPhase = 2;
            }
          }
        }
      }
      else
      {
        if (SD_GetStatusFlags() & 0x80)
        {
          result = (u8)gModel_aSlots[0].is_active;
          if ( gModel_aSlots[0].is_active )
          {
            result = D_8009B004.fields.credits_update_enabled + 1;
            ++D_8009B004.fields.credits_update_enabled;
          }
        }
      }
      return;
}

void Model_StartBattleSlotAction(s32 arg0)
{
    ModelSlot *slot;
    ModelSlotMetadataBlock *metadata;
    s32 action_row;
    s32 camera_duration;

    slot = &gModel_aSlots[arg0];
    metadata = &gModel_aSlots[arg0].metadata;
    action_row = slot->guardian_star_selection + 3;
    camera_duration = slot->animation_duration_rows[action_row].max;
    if (camera_duration != 0) {
        Model_ControlSlotAnimation(arg0, action_row, 1);
    }
    camera_duration = camera_duration * 2 / 3;
    if (camera_duration < 60) {
        camera_duration = 60;
    }
    if (metadata->battle_camera_duration_overrides[slot->guardian_star_selection] != 0) {
        camera_duration = metadata->battle_camera_duration_overrides[slot->guardian_star_selection];
    }
    Model_RequestCameraMoveToSlots(1, -1, arg0 ^ 1, camera_duration);
    Model_SetCameraHoldState(
        ((metadata->prefix.bytes.guardian_star_options[slot->guardian_star_selection] & 0x80) == 0) << 1
    );
    if ((metadata->prefix.bytes.guardian_star_options[slot->guardian_star_selection] & 0x40) != 0) {
        Model_SetCameraHoldState(1);
    }
}

/* "e%03ld(%ld)=%d\n" */
void Model_PlaySlotAnimationSounds(s32 index) {
#ifndef VERSION_JAPAN
    /* The Japanese build has no debug print, and no buffer for it. */
    u8 trace_text[0x50];
#endif
    u8 spatial_bytes[8];
    ModelSlot *slot;
    ModelSlotSoundEntry *cue;
    s32 animation_row;
    s32 row_end_position;
    s32 position_step;
    s32 sound_position_step;
    s32 sound_id;
    s32 cue_position;
    s32 slot_sound_tag;
    s32 use_command_cursor;
    s32 cue_index;
    s32 selected_command;
    s32 guardian_star;
    s32 sound_command;
    u16 cd_seek_flag;

    slot = &gModel_aSlots[index];
    if (slot->is_active == 0) {
        return;
    }
    if (slot->tint[3] >= 2) {
        if ((*(u32 *)slot->tint & 0xFFFFFF) == 0) {
            return;
        }
    }
    if (slot->animation_sound_enabled == 0) {
        return;
    }
    animation_row = slot->current_animation_row;
    row_end_position = slot->animation_duration_rows[animation_row].max << 4;
    position_step = slot->playback_speed * Model_GetFrameStep();
    cue = slot->sound_entries;
    slot_sound_tag = (index << 8) + 0x4000;
    if (row_end_position != 0) {
        cue_position = slot->animation_position % row_end_position;
    } else {
        cue_position = 0;
    }
    guardian_star = slot->guardian_star_selection;
    selected_command = ((ModelControlCommandView *)slot)->commands[guardian_star];
    use_command_cursor = 0;
    if (selected_command >= 0) {
        if (slot->animation_control_state != 8) {
            if (slot->animation_control_state != 2 || guardian_star + 3 == animation_row) {
                use_command_cursor = 1;
            }
        }
        if (use_command_cursor) {
#ifdef VERSION_JAPAN
            sound_position_step = slot->playback_speed * Model_GetFrameStep();
            cue_position = slot->animation_sound_position;
            slot->animation_sound_position = cue_position + sound_position_step;
            animation_row = slot->guardian_star_selection + 3;
#else
            s32 trace_star;
            s32 trace_command;

            sound_position_step = slot->playback_speed * Model_GetFrameStep();
            trace_star = slot->guardian_star_selection;
            cue_position = slot->animation_sound_position;
            slot->animation_sound_position = cue_position + sound_position_step;
            trace_command = ((ModelControlCommandView *)slot)->commands[trace_star];
            animation_row = slot->guardian_star_selection + 3;
            /* Retail loads animation_sound_position again after the store above; read
               plainly, GCC reuses the value it just stored instead. */
            sprintf(trace_text, gModel_szAnimationSoundTraceFormat, trace_command / 1000, trace_command % 1000, *(volatile u16 *)&slot->animation_sound_position >> 4);
            FntPrint(trace_text);
#endif
        }
    }
    if (use_command_cursor == 0) {
        if (slot->playback_state == 0x23) {
            return;
        }
    }
    SD_CalcSpatialVolumePan(
        slot->cached_position[0], slot->cached_position[2], &spatial_bytes[0], &spatial_bytes[1]
    );
    cue_index = 0;
loop:
    cd_seek_flag = cue->flags & 0x8000;
    sound_id = cue->sound_id;
    /* A one-pass block around the loop body; it decides which of cue_position
     * and the counter get $s1 and $s3. */
    do {
        if (cd_seek_flag) {
            sound_command = cd_seek_flag | sound_id;
        } else {
            sound_command = (u16)slot_sound_tag | sound_id;
        }
        if (cue->animation_row == 0) {
            return;
        }
        if (cue->animation_row == animation_row) {
            if (cd_seek_flag != 0 && cue_position == 0) {
                SD_EnqueueCdSeek(sound_command);
            }
            if ((cue->flags & 0x7FFF) >= cue_position && (cue->flags & 0x7FFF) < cue_position + position_step) {
                SD_SEPlay(sound_command, 0xFF, 0);
            }
        }
        cue_index++;
        cue++;
    } while (0);
    if (cue_index < 0x40) {
        goto loop;
    }
}

#define MODEL_TINT_COLOR_VIEW(color) ((ModelTintColor *)(color))
#define SVECTOR_VIEW(vector) ((SVECTOR *)(vector))
#define MODEL_SLOT_BYTES(slots, index) \
    ((u8 *)(slots) + (index) * MODEL_SLOT_SIZE)

/* The contiguous model camera and scene runtime. Model_SeparateViewFromSlots pushes the
   gModel_RView reference apart from the two model records; Model_UpdateCamera
   steps the camera move at gModel_CameraMove each frame and calls the second
   camera/view correction pass, Model_CorrectViewDistance, which in turn calls
   Model_SeparateViewFromSlots. The two correction passes work on gModel_RView, gModel_CameraMove
   and the gModel_wViewDistance/gModel_wViewPitch view values; the tint
   request pass Model_UpdateTintRequests follows them; camera-move setup
   (Model_StartCameraMove) and scene reset/configuration close the unit. */

/*
 * Recursively separates the two model records when their projected distance
 * is below the largest paired half-extent, pushing gModel_RView apart and
 * retrying up to depth three. Matching C under gcc_2_8_1_g8_split with no
 * register pins, inline assembly or symbol aliases.
 *
 * Two behavioural points read off the retail listing:
 *  - `moved = limit` is set after the main body as well as on the mode == 0
 *    path (retail's `j next` carries `move s1,s5` in its slot), so the loop
 *    is `if (moved) { hits++; continue; } if (mode != 0) { body } moved =
 *    limit;`;
 *  - the push is scaled by the extents, `px = e0.values[i] * scale / 4096`
 *    and `pz = e2.values[i] * scale / 4096`; dx[i] and dz[i] decide only the
 *    sign.
 *
 * Shape that the compiler needs, in the order it was found:
 *  - each clamp pair is written into the struct declared after its
 *    destination and copied down (`e3 = e0`, `e0 = e1`, ...), with a
 *    do { } while (0) around the store before each copy: the loop notes are
 *    sched1 barriers that keep retail's global loads below the pair copies;
 *  - the dead read `nv = gModel_RView.vpx` above the last pin, and the three
 *    reference names ref/nv/ry, leave the `lui` of gModel_RView as the join
 *    block's first insn so reorg can hoist it into the beqz slot;
 *  - the `hits++` arm sits out of line after the loop bottom;
 *  - `sd = ... / len` sits in its own do { } while (0), `cpx`/`apz` are
 *    named before the length call, and `cross` after them;
 *  - the loop-top reads of dy.values[i] and e1.values[i] go through two
 *    pointers that start at the frame base and are stepped twice, and the
 *    limit comparison is stored back into eb; see the comments in the loop.
 *
 * See notes/research/func-80051350-decode.md for the structural map.
 */
s32 Model_SeparateViewFromSlots(s32 mode, s32 min_extent, s32 depth)
{
    ModelSeparationPair e3;
    ModelSeparationPair e0;
    ModelSeparationPair e1;
    ModelSeparationPair e2;
    ModelSeparationPair dx;
    ModelSeparationPair dy;
    ModelSeparationPair dz;
    ModelSeparationPair dist;
    ModelSeparationPair t;
    s32 moved;
    s32 hits;
    s32 i;
    s32 ox;
    s32 oz;
    s32 ref;
    s32 nv;
    s32 ry;
    s32 dz1;
    s32 v;
    s32 v5;
    ModelSlot *rec;

    ox = rcos(*(s16 *)&gModel_wViewYaw + 0x800) * min_extent / 4096;
    oz = rsin(*(s16 *)&gModel_wViewYaw + 0x800) * min_extent / 4096;

    rec = gModel_aSlots;
    v = (s16)rec[0].cached_dimensions[3] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e0.values[0] = v;
    v = (s16)rec[1].cached_dimensions[3] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    do {
        e0.values[1] = v;
    } while (0);
    e3 = e0;
    v = (s16)rec[0].cached_dimensions[0] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e1.values[0] = v;
    v = (s16)rec[1].cached_dimensions[0] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    do {
        e1.values[1] = v;
    } while (0);
    e0 = e1;
    v = (s16)rec[0].cached_dimensions[1] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e2.values[0] = v;
    v = (s16)rec[1].cached_dimensions[1] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    do {
        e2.values[1] = v;
    } while (0);
    e1 = e2;
    v = (s16)rec[0].cached_dimensions[2] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    dx.values[0] = v;
    v5 = (s16)rec[1].cached_dimensions[2] / 2;
    if (v5 < min_extent) {
        v5 = min_extent;
    }
    nv = gModel_RView.vpx;
    do {
        dx.values[1] = v5;
    } while (0);
    e2 = dx;

    ref = gModel_RView.vpx;
    ref = ref + ox;
    dy.values[0] = ref - *(s16 *)&rec[0].cached_position[0];
    dy.values[1] = ref - *(s16 *)&rec[1].cached_position[0];
    dx = dy;
    nv = gModel_RView.vpy;
    dz.values[0] = nv - *(s16 *)&rec[0].cached_position[1];
    dz1 = nv - *(s16 *)&rec[1].cached_position[1];
    do {
        dz.values[1] = dz1;
    } while (0);
    dy = dz;
    ry = gModel_RView.vpz + oz;
    dist.values[0] = ry - *(s16 *)&rec[0].cached_position[2];
    dist.values[1] = ry - *(s16 *)&rec[1].cached_position[2];
    dz = dist;

    t.values[0] = SquareRoot0(
        dx.values[0] * dx.values[0] + dz.values[0] * dz.values[0]);
    t.values[1] = SquareRoot0(
        dx.values[1] * dx.values[1] + dz.values[1] * dz.values[1]);
    dist = t;

    moved = 0;
    hits = 0;
    if (gModel_aSlots[0].is_active != 0) {
        if (gModel_aSlots[0].tint[3] >= 2) {
            if ((*(u32 *)gModel_aSlots[0].tint & 0xFFFFFF) == 0) {
                dist.values[0] = -1;
            }
        }
    } else {
        dist.values[0] = -1;
    }
    if (gModel_aSlots[1].is_active != 0) {
        if (gModel_aSlots[1].tint[3] >= 2) {
            if ((*(u32 *)gModel_aSlots[1].tint & 0xFFFFFF) == 0) {
                dist.values[1] = -1;
            }
        }
    } else {
        dist.values[1] = -1;
    }

    depth = depth + 1;
    for (i = 0; i < 2; i++) {
        s32 limit;
        s32 eb;
        s32 d;
        s32 v;
        s32 *pv;
        s32 *pb;

        limit = e2.values[i];
        if (limit < e0.values[i]) {
            limit = e0.values[i];
        }
        if (limit < e3.values[i]) {
            limit = e3.values[i];
        }
        /* dy.values[i] and e1.values[i] through two pointers that start at
         * the frame base (e3 is the first pair, at sp+16) and step by i and
         * then by one. The final step folds into the load offsets, but it
         * leaves each pointer with two sets, so neither address insn gets
         * sched1's launch priority and both are computed before the first
         * load; reload_cse then turns the second into retail's copy. */
        pv = e3.values - 4;
        pv += i;
        pv++;
        pb = e3.values - 4;
        pb += i;
        pb++;
        v = pv[13];
        eb = pb[7];
        if (v < 0) {
            v = -v;
        }
        /* The comparison lands in eb itself: a block-local flag would be
         * placed first by local-alloc and take v0, pushing v to v1. */
        eb = eb < v;
        if (eb) {
            continue;
        }
        d = dist.values[i];
        if (d < 0) {
            continue;
        }
        if (d >= limit) {
            continue;
        }
        if (moved != 0) {
            goto hit;
        }
        if (mode != 0) {
            s32 scale;
            s32 px;
            s32 pz;

            scale = ((limit - d) << 12) / limit;
            px = e0.values[i] * scale / 4096;
            if (dx.values[i] <= 0) {
                px = -px;
            }
            pz = e2.values[i] * scale / 4096;
            if (dz.values[i] <= 0) {
                pz = -pz;
            }
            {
                s32 ax = gModel_RView.vrx;
                s32 az = gModel_RView.vpz;
                s32 bx = gModel_RView.vrz;
                s32 cx = gModel_RView.vpx;
                s32 ux = bx - az;
                s32 uz = cx - ax;
                s32 cpx = cx + px;
                s32 apz = az + pz;
                s32 cross = ax * az - cx * bx;
                s32 len = SquareRoot0(ux * ux + uz * uz);
                s32 sd = 0;

                if (len != 0) {
                    do {
                        sd = (cpx * ux + apz * uz + cross) / len;
                    } while (0);
                }
                if (gModel_bOrbitDirectionTimer == 0) {
                    if (sd < 0) {
                        gModel_bOrbitDirection = -1;
                    } else {
                        gModel_bOrbitDirection = 1;
                    }
                    gModel_bOrbitDirectionTimer = 0x1E;
                } else if (gModel_bOrbitDirectionTimer < 0xFF) {
                    gModel_bOrbitDirectionTimer = gModel_bOrbitDirectionTimer - 1;
                }
            }
            gModel_RView.vpx = gModel_RView.vpx + px;
            gModel_RView.vpz = gModel_RView.vpz + pz;
        }
        moved = limit;
        continue;
    hit:
        hits = hits + 1;
    }

    if (mode != 0 && moved != 0) {
        Model_UpdateViewMetrics(0);
    }
    if (hits != 0 && mode != 0 && depth < 3) {
        Model_SeparateViewFromSlots(mode, min_extent, depth);
    }
    return moved;
}

void Model_UpdateCamera(void)
{
    s32 eye_slots[2] = {gModel_CameraMove.eye.pair_slot, gModel_CameraMove.eye.slot};
    s32 target_slots[2] = {gModel_CameraMove.target.pair_slot, gModel_CameraMove.target.slot};
    s32 rate;
    s32 progress;
    Key *key;

    if (Model_IsEffectKeyQueueEmpty() && gModel_CameraMove.flags) {
        if (gModel_CameraMove.duration <= 0xFFFE) {
            s32 next_elapsed;

            rate = Model_GetFrameStep();
            if (gModel_CameraMove.elapsed < (u16)(gModel_CameraMove.duration / 5u) ||
                gModel_CameraMove.elapsed > 4 * gModel_CameraMove.duration / 5) {
                rate /= 2;
                if (rate < 2)
                    rate = 2;
            }
            next_elapsed = gModel_CameraMove.elapsed + rate;
            gModel_CameraMove.elapsed = next_elapsed < gModel_CameraMove.duration ?
                next_elapsed : gModel_CameraMove.duration;
        }
        if (eye_slots[0] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;

            slot = base + eye_slots[0];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.eye.start_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.eye.start_y;
            gModel_CameraMove.eye.start_x = gModel_CameraMove.eye.start_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.eye.start_y = gModel_CameraMove.eye.start_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
        }
        if (eye_slots[1] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;
            s32 dz;

            slot = base + eye_slots[1];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.eye.end_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.eye.end_y;
            dz = (s16)slot->cached_position[2] - gModel_CameraMove.eye.end_z;
            gModel_CameraMove.eye.end_x = gModel_CameraMove.eye.end_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.eye.end_y = gModel_CameraMove.eye.end_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
            gModel_CameraMove.eye.end_z = gModel_CameraMove.eye.end_z +
                (dz > 0 ? (dz < 31 ? dz : 30) :
                (dz >= -30 ? dz : -30));
        }
        if (target_slots[0] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;

            slot = base + target_slots[0];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.target.start_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.target.start_y;
            gModel_CameraMove.target.start_x = gModel_CameraMove.target.start_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.target.start_y = gModel_CameraMove.target.start_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
        }
        if (target_slots[1] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;
            s32 dz;

            slot = base + target_slots[1];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.target.end_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.target.end_y;
            dz = (s16)slot->cached_position[2] - gModel_CameraMove.target.end_z;
            gModel_CameraMove.target.end_x = gModel_CameraMove.target.end_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.target.end_y = gModel_CameraMove.target.end_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
            gModel_CameraMove.target.end_z = gModel_CameraMove.target.end_z +
                (dz > 0 ? (dz < 31 ? dz : 30) :
                (dz >= -30 ? dz : -30));
        }
        if (gModel_CameraMove.flags & 1) {
            gModel_RView.vpx = (gModel_CameraMove.eye.end_x - gModel_CameraMove.eye.start_x) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.eye.start_x;
            gModel_RView.vpy = (gModel_CameraMove.eye.end_y - gModel_CameraMove.eye.start_y) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.eye.start_y;
            gModel_RView.vpz = (gModel_CameraMove.eye.end_z - gModel_CameraMove.eye.start_z) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.eye.start_z;
            if (gModel_RView.vpy >= -100)
                gModel_RView.vpy = -100;
        } else if (gModel_CameraMove.mode) {
            rate = Model_GetFrameStep();
            Model_StepViewOrbit(1, gModel_CameraMove.orbit_step * rate * gModel_bOrbitDirection, 0, 0, 0);
        }
        if (gModel_CameraMove.flags & 2) {
            gModel_RView.vrx = (gModel_CameraMove.target.end_x - gModel_CameraMove.target.start_x) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.target.start_x;
            gModel_RView.vry = (gModel_CameraMove.target.end_y - gModel_CameraMove.target.start_y) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.target.start_y;
            gModel_RView.vrz = (gModel_CameraMove.target.end_z - gModel_CameraMove.target.start_z) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.target.start_z;
            if (gModel_RView.vry >= -100)
                gModel_RView.vry = -100;
        }
        Model_UpdateViewMetrics(0);
        Model_CorrectViewDistance(1);
        if (gModel_CameraMove.elapsed >= gModel_CameraMove.duration)
            Model_RequestCameraMove(1);
        return;
    }
    if (!gModel_CameraMove.mode)
        return;
    key = (Key *)Model_GetCurrentEffectKeyAddress();
    if (eye_slots[1] >= 0) {
        ModelSlot *base = gModel_aSlots;
        ModelSlot *slot;
        s32 dx;
        s32 dy;
        s32 dz;

        slot = base + eye_slots[1];
        dx = (s16)slot->cached_position[0] - gModel_RView.vpx;
        dy = (s16)slot->cached_position[1] - gModel_RView.vpy;
        dz = (s16)slot->cached_position[2] - gModel_RView.vpz;
        gModel_RView.vpx = gModel_RView.vpx +
            (dx > 0 ? (dx < 31 ? dx : 30) :
            (dx >= -30 ? dx : -30));
        gModel_RView.vpy = gModel_RView.vpy +
            (dy > 0 ? (dy < 31 ? dy : 30) :
            (dy >= -30 ? dy : -30));
        gModel_RView.vpz = gModel_RView.vpz +
            (dz > 0 ? (dz < 31 ? dz : 30) :
            (dz >= -30 ? dz : -30));
        if (gModel_RView.vpy >= -100)
            gModel_RView.vpy = -100;
    }
    if (target_slots[1] >= 0) {
        ModelSlot *base = gModel_aSlots;
        ModelSlot *slot;
        s32 dx;
        s32 dy;
        s32 dz;

        slot = base + target_slots[1];
        dx = (s16)slot->cached_position[0] - gModel_RView.vrx;
        dy = (s16)slot->cached_position[1] - gModel_RView.vry;
        dz = (s16)slot->cached_position[2] - gModel_RView.vrz;
        gModel_RView.vrx = gModel_RView.vrx +
            (dx > 0 ? (dx < 31 ? dx : 30) :
            (dx >= -30 ? dx : -30));
        gModel_RView.vry = gModel_RView.vry +
            (dy > 0 ? (dy < 31 ? dy : 30) :
            (dy >= -30 ? dy : -30));
        gModel_RView.vrz = gModel_RView.vrz +
            (dz > 0 ? (dz < 31 ? dz : 30) :
            (dz >= -30 ? dz : -30));
        if (gModel_RView.vry >= -100)
            gModel_RView.vry = -100;
    }
    if (!key || (key->requested[0].kind != 4 && key->requested[1].kind != 4)) {
        rate = Model_GetFrameStep();
        Model_StepViewOrbit(1, gModel_CameraMove.orbit_step * rate * gModel_bOrbitDirection, 0, 0, 0);
        Model_CorrectViewDistance(1);
    }
    if (gModel_CameraMove.follow_countdown) {
        ModelCameraMove *move = &gModel_CameraMove;
        s32 step;

        step = Model_GetFrameStep();
        if (move->follow_countdown - step > 0) {
            step = Model_GetFrameStep();
            progress = move->follow_countdown - step;
        } else {
            progress = 0;
        }
        move->follow_countdown = progress;
    }
}

void Model_CorrectViewPitch(void) {
    GsRVIEW2 *view;
    s32 x;
    s32 f;
    s32 v;
    s32 lo;
    s32 c;
    s32 d;

    if (*(s16 *)&gModel_wViewPitch >= 0x801) {
        view = &gModel_RView;
        x = view->vry;
        f = 0;
        if (x >= -0xC7 && view->vpy < x) {
            if (x < -0x64) {
                f = (-200 - x) * MODEL_FIXED_ONE / -100;
            } else {
                f = MODEL_FIXED_ONE;
            }
        }

        v = (f * 3755 + (MODEL_FIXED_ONE - f) * 3968) /
            MODEL_FIXED_ONE;
        lo = v - 0x10;

        do {
            c = *(s16 *)&gModel_wViewPitch;
            d = v + 0x10;
        } while (0);

        if (d < c) {
            d = d - c;
            if (d < -1) {
                d = -1;
            }
            Model_StepViewOrbit(1, 0, d, 0, 0);
        } else if (c < 0xEAB) {
            Model_StepViewOrbit(1, 0, 0xEAB - c, 0, 0);
        } else if (c < lo) {
            d = lo - c;
            if (d >= 5) {
                d = 4;
            }
            Model_StepViewOrbit(1, 0, d, 0, 0);
        }
    } else {
        Model_StepViewOrbit(1, 0, -4, 0, 0);
    }
}

void Model_CorrectViewDistance(s32 arg0) {
    ModelCameraMove *e;
    ModelCameraMove *g;
    s32 n;
    s32 lo;
    s32 hi;
    s32 z;
    s32 d;
    s32 t;

    n = gModel_wCameraTargetSlot[0];
    lo = 0x2BC;
    hi = 0x3E8;
    z = 0;

    if (n >= 0) {
        lo = Model_AdjustCameraDistanceForSlot(n, lo);
        if (lo >= hi) {
            hi = Model_AdjustCameraDistanceForSlot(n, hi);
        }
    }

    /* The Japanese build only clears the flag bit here; it neither steps
       orbit_step up nor, on the other branch, back down towards base_orbit_step. */
    if (Model_SeparateViewFromSlots(1, 0x96, 0) != 0) {
        e = &gModel_CameraMove;
#ifdef VERSION_EUROPE
        /* The European build steps orbit_step by 2 up to 0x200, and back
           down by 2 without passing base_orbit_step. */
        if (e->orbit_step < 0x200) {
            e->orbit_step = e->orbit_step + 2;
        }
#elif !defined(VERSION_JAPAN)
        if (e->orbit_step < 0x10) {
            e->orbit_step = e->orbit_step + 1;
        }
#endif
        e->flags = e->flags & 0xFE;
    }
#ifdef VERSION_EUROPE
    else {
        u16 floor;
        u16 v;

        g = &gModel_CameraMove;
        floor = g->base_orbit_step;
        if (floor < g->orbit_step) {
            v = g->orbit_step - 2;
            g->orbit_step = v;
            if (v < floor) {
                g->orbit_step = floor;
            }
        }
    }
#elif !defined(VERSION_JAPAN)
    else {
        g = &gModel_CameraMove;
        if (g->base_orbit_step < g->orbit_step) {
            g->orbit_step = g->orbit_step - 1;
        }
    }
#endif

    Model_CorrectViewPitch();

    t = *(s16 *)&gModel_wViewDistance;
    if (t < 0x12C) {
        if (t < 0x96) {
            d = 0x96 - t;
            Model_StepViewOrbit(1, z, 0, d, 0);
        } else {
            d = 0x12C - t;
            if (d >= 0xB) {
                d = 0xA;
            }
            Model_StepViewOrbit(1, z, 0, d, 0);
        }
    } else if (t < lo) {
        d = lo - t;
        if (d >= 6) {
            d = 5;
        }
        Model_StepViewOrbit(1, 0, 0, d, 0);
    }

    if (arg0 != 0) {
        d = *(s16 *)&gModel_wViewDistance;
        if (hi < d) {
            goto last;
        }
        if (lo < d) {
            d = lo - d;
            if (d < -0xA) {
                d = -0xA;
            }
            Model_StepViewOrbit(1, 0, 0, d, 0);
        }
    }

    if (hi < *(s16 *)&gModel_wViewDistance) {
last:
        d = hi - *(s16 *)&gModel_wViewDistance;
        if (d < -0x14) {
            d = -0x14;
        }
        Model_StepViewOrbit(1, 0, 0, d, 0);
    }

    Model_CorrectViewPitch();
}

/* Per-frame tint pass over the ten requests at gModel_aTintRequests. A live request
 * interpolates its start colour towards its end colour, redraws the slot with
 * the saved animation selector, then restores the slot and advances the request.
 *
 * What the allocation rests on: the shifted flag word `v` is masked into its
 * own name, `part`, ahead of the Model_SetSlotShadowEnabled call, and `part`
 * is copied to `sid` after it. `part` therefore lives and dies inside the one
 * basic block while crossing the call, so local-alloc gives the flag word,
 * shift and mask $s2 and pushes animation_position_snapshot to $s3; `sid` then inherits $s2
 * through its
 * copy preference. `v` itself never crosses the call, which is what holds the
 * srl and andi ahead of it where the scheduler would otherwise sink them to
 * the end of the block. The saved current_animation_row is one name, `keep`, across both
 * loops. */
void Model_UpdateTintRequests(void)
{
    ModelTintColor save;
    ModelTintColor col;
    s32 i;
    s32 j;
    s32 k;
    u32 v;
    s32 keep;
    s32 rowbase;
    u32 part;
    u32 sid;
    s32 old;
    u8 side;
    u16 sav06;
    s32 off;
    u16 lo;
    u16 hi;
    u16 a;
    s32 aa;
    s32 sv;
    ModelSlot *slot;
    ModelTintRequest *e;
    ModelTintRequest *table;

    table = (ModelTintRequest *)gModel_aTintRequests;
    for (i = 0, off = 0; i < MODEL_TINT_REQUEST_COUNT; off += 0x18, i++) {
        e = &table[i];
        rowbase = (s32)((u8 *)table + 1);
        if ((*(u8 *)e & 1) == 0) {
            continue;
        }
        if (gModel_aSlots[(e->flags >> 1) & 1].is_active == 0) {
            continue;
        }
        if (Model_HasInsufficientBufferSpace((e->flags >> 1) & 1, e->start.blend_mode)) {
            goto tail;
        }
        v = e->flags;
        a = e->animation_position_snapshot;
        lo = e->elapsed;
        hi = e->duration;
        side = (v >> 1) & 1;
        v = v >> 3;
        slot = &gModel_aSlots[side];
        sav06 = slot->animation_position;
        part = v & 0x1F;
        keep = slot->current_animation_row;
        old = Model_SetSlotShadowEnabled(side, 0);
        sid = part;
        save = *MODEL_TINT_COLOR_VIEW(slot->tint);
        col.blend_mode = e->start.blend_mode;
        col.r = e->start.r * (hi - lo) / hi + e->end.r * lo / hi;
        col.g = e->start.g * (hi - lo) / hi + e->end.g * lo / hi;
        col.b = e->start.b * (hi - lo) / hi + e->end.b * lo / hi;
        *MODEL_TINT_COLOR_VIEW(slot->tint) = col;

        aa = a;
        for (j = 0; j < slot->animation_sequence_count; j++) {
            s32 t = slot->current_animation_row;
            if (sid != 0) {
                slot->animation_sequences[j]->sid = sid;
                t = sid;
            }
            Model_SeekSlotPartAnimation(slot, j, t, aa);
        }
        if (sid != 0) {
            slot->current_animation_row = sid;
        }

        gModel_PartMaskAddress = off + rowbase;
        gModel_bDrawMaskedPartsOnly = (e->flags >> 2) & 1;
        Model_DrawSlot(side);
        gModel_PartMaskAddress = 0;
        gModel_bDrawMaskedPartsOnly = 0;

        sv = sav06;
        for (k = 0; k < slot->animation_sequence_count; k++) {
            s32 t = slot->current_animation_row;
            if (keep != 0) {
                slot->animation_sequences[k]->sid = keep;
                t = keep;
            }
            Model_SeekSlotPartAnimation(slot, k, t, sv);
        }
        if (keep != 0) {
            slot->current_animation_row = keep;
        }

        Model_SetSlotShadowEnabled(side, old);
        *MODEL_TINT_COLOR_VIEW(slot->tint) = save;
tail:
        e->elapsed += Model_GetFrameStep();
        if (e->elapsed >= e->duration) {
            e->flags &= 0xFFFE;
        }
    }
}

/* Starts a camera move from the live view to model-slot or explicit targets.
 * A zero duration writes the selected endpoints back immediately. */
void Model_StartCameraMove(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    SVECTOR a;
    SVECTOR b;
    SVECTOR t;
    s32 index;
    s32 state;
    s32 flags;

    memset(&b, 0, 8);
    b.vx = gModel_RView.vpx;
    b.vy = gModel_RView.vpy;
    b.vz = gModel_RView.vpz;
    a = b;
    memset(&t, 0, 8);
    t.vx = gModel_RView.vrx;
    t.vy = gModel_RView.vry;
    t.vz = gModel_RView.vrz;
    b = t;
    state = Model_GetCameraHoldState();
    if (state == 1) {
        if (Model_GetHandlerCallbackState() == state) {
            return;
        }
    }
    if (arg1 != 0 || arg0 <= 0) {
        if (arg1 > 0) {
            index = arg1 - 1;
            arg1 = (s32)MODEL_SLOT_BYTES(gModel_awSlot0Position, index);
            if (arg3 >= 0) {
                gModel_CameraMove.eye.pair_slot = index ^ 1;
            } else {
                gModel_CameraMove.eye.pair_slot = -1;
            }
            gModel_CameraMove.eye.slot = index;
        } else {
            ModelCameraMove *p = &gModel_CameraMove;

            p->eye.slot = -1;
            p->eye.pair_slot = -1;
        }
    }
    if (arg2 != 0 || arg0 <= 0) {
        if (arg2 > 0) {
            index = arg2 - 1;
            arg2 = (s32)MODEL_SLOT_BYTES(gModel_awSlot0Position, index);
            if (arg3 >= 0) {
                gModel_CameraMove.target.pair_slot = index ^ 1;
            } else {
                gModel_CameraMove.target.pair_slot = -1;
            }
            gModel_CameraMove.target.slot = index;
        } else {
            ModelCameraMove *p = &gModel_CameraMove;

            p->target.slot = -1;
            p->target.pair_slot = -1;
        }
        if (arg0 < 0) {
            goto have_flags;
        }
    }
    gModel_CameraMove.mode = arg0;
have_flags:
    {
        ModelCameraMove *p = &gModel_CameraMove;

        p->follow_countdown = MODEL_DEFAULT_PROJECTION;
#ifndef MODEL_CAMERA_MOVE_COMPACT_LAYOUT
        p->base_orbit_step = 8;
#endif
        p->orbit_step = 8;
        p->flags = 0;
        if (arg1 != 0) {
            p->flags = 1;
        } else {
            arg1 = (s32)&a;
        }
    }
    if (arg2 != 0) {
        ModelCameraMove *p = &gModel_CameraMove;

        p->flags = p->flags | 2;
    } else {
        arg2 = (s32)&b;
    }
    {
        ModelCameraMove *p = &gModel_CameraMove;

        p->eye.start_x = a.vx;
        p->eye.start_y = a.vy;
        p->eye.start_z = a.vz;
        /* arg1/arg2 now hold a slot's cached_position position or &a/&b; both
           are SVECTOR-shaped. */
        p->eye.end_x = SVECTOR_VIEW(arg1)->vx;
        p->eye.end_y = SVECTOR_VIEW(arg1)->vy;
        p->eye.end_z = SVECTOR_VIEW(arg1)->vz;
        p->target.start_x = b.vx;
        p->target.start_y = b.vy;
        p->target.start_z = b.vz;
        p->target.end_x = SVECTOR_VIEW(arg2)->vx;
        p->target.end_y = SVECTOR_VIEW(arg2)->vy;
        p->target.end_z = SVECTOR_VIEW(arg2)->vz;
        arg3 = (arg3 < 0 ? -arg3 : arg3) * 2;
        if (arg3 > 0xFFFE) {
            p->duration = 0xFFFF;
        } else {
            p->duration = arg3;
        }
    }
    {
        ModelCameraMove *p = &gModel_CameraMove;

        flags = p->flags;
        p->elapsed = 0;
        if (flags != 0) {
            gModel_bOrbitDirectionTimer = 0;
        }
        if (arg3 != 0) {
            return;
        }
        if (flags & 1) {
            gModel_RView.vpx = p->eye.end_x;
            gModel_RView.vpy = p->eye.end_y;
            gModel_RView.vpz = p->eye.end_z;
        }
        if (p->flags & 2) {
            gModel_RView.vrx = p->target.end_x;
            gModel_RView.vry = p->target.end_y;
            gModel_RView.vrz = p->target.end_z;
        }
        if (p->flags & 3) {
            Model_UpdateViewMetrics(0);
        }
        p->flags = 0;
    }
}

void Model_ResetSceneView(void)
{
    int projection_distance = gModel_wProjectionDistance;
    gModel_wBackgroundOffsetY = 0;
    gModel_wBackgroundScrollX = 0;
    gModel_wViewPitch = 0;
    gModel_wViewDistance = 0;
    gModel_wViewYaw = 0xC00;
    GsSetProjection(projection_distance);
    gModel_RView.vpx = 0;
    gModel_RView.vpy = -350;
    gModel_RView.vpz = -1200;
    gModel_RView.vrx = 0;
    gModel_RView.vry = -350;
    gModel_RView.vrz = 0;
    gModel_RView.rz = 0;
    gModel_RView.super = 0;
    Model_StepViewOrbit(1, 0x80, -85, 1200, 0);
    GsSetRefView2(&gModel_RView);
    {
        GsCOORDUNIT *camera_coordinate = &gModel_CameraCoordinateUnit;
        camera_coordinate->rot.vz = 0;
        camera_coordinate->rot.vy = 0;
        camera_coordinate->rot.vx = 0;
        camera_coordinate->matrix.t[2] = 0;
        camera_coordinate->matrix.t[1] = 0;
        camera_coordinate->matrix.t[0] = 0;
        camera_coordinate->super = 0;
        RotMatrix_gte(&camera_coordinate->rot, &camera_coordinate->matrix);
        camera_coordinate->flg = 0;
    }
    gModel_SavedRefView = gModel_RView;
    gModel_ViewSnapshot = D_8009B478_p;
    Model_UpdateBackgroundScroll(1);
    {
        int camera_hold_state = Model_GetCameraHoldState();
        if (camera_hold_state != 1 || Model_GetHandlerCallbackState() != camera_hold_state)
            Model_StartCameraMove(0, 0, 0, 0);
    }
    Model_ResetEffectKeyQueue();
}

/* The Japanese build sets only the slot record, without the three
   gModel_awSlotModelIds/gModelDebug_abSlotAfFlags/gModelDebug_abSlotGfFlags mirrors. */
void Model_SetSlotProperties(s32 idx, ...)
{
    va_list arguments;
    s32 model_id;
    s32 attack_override;
    s32 defense_override;
    s32 guardian_star_option;
    s32 defense_position_option;

    va_start(arguments, idx);
    model_id = va_arg(arguments, s32);
    if (model_id >= 0) {
        gModel_aSlots[idx].model_id = model_id;
#ifndef VERSION_JAPAN
        gModel_awSlotModelIds[idx] = model_id;
#endif
    }
    if (idx < 2) {
        attack_override = va_arg(arguments, s32);
        defense_override = va_arg(arguments, s32);
        guardian_star_option = va_arg(arguments, s32);
        defense_position_option = va_arg(arguments, s32);
        if (attack_override >= 0) {
            gModel_aSlots[idx].attack_compare_override = attack_override;
        }
        if (defense_override >= 0) {
            gModel_aSlots[idx].defense_compare_override = defense_override;
        }
        if (guardian_star_option >= 0) {
            gModel_aSlots[idx].guardian_star_selection = (guardian_star_option != 0);
#ifndef VERSION_JAPAN
            gModelDebug_abSlotAfFlags[idx] = (guardian_star_option != 0);
#endif
        }
        if (defense_position_option >= 0) {
            gModel_aSlots[idx].defense_position = (defense_position_option != 0);
#ifndef VERSION_JAPAN
            gModelDebug_abSlotGfFlags[idx] = (defense_position_option != 0);
#endif
        }
    } else {
        gModel_pBackgroundRecord =
            (ModelBackgroundRecord *)&gModel_abBackgroundRecords[gModel_awStageSlotModelId[0] * 0xB2];
    }
    gModel_bSceneMode = 15;
}

/* Regional value: the x of the row Model_ResetSceneState clears, 0x200 in the
   US build and 0x280 in the European one, whose wrapper defines its own. */
#ifndef MODEL_TEXTURE_RESET_ROW_X
#define MODEL_TEXTURE_RESET_ROW_X 0x200
#endif

void Model_ResetSceneState(void){s16 table[256];RECT packet;register s16*p=&table[1];register s32 fill=0xffff;register s32 counter=254;s32 i;table[0]=0;do{*p=fill;counter--;p++;}while(counter>=0);packet.x=MODEL_TEXTURE_RESET_ROW_X;packet.y=0xF0;packet.w=0x100;packet.h=1;while(IsIdleGPU(3)){}while(LoadImage2(&packet,(u32 *)table)){}while(IsIdleGPU(3)){}Model_ResetSlot(0);Model_ResetSlot(1);Model_ResetSlot(2);for(i=0;i<MODEL_TINT_REQUEST_COUNT;i++)gModel_aTintRequests[i].flags&=0xfffe;gModel_bDrawMaskedPartsOnly=0;gModel_PartMaskAddress=0;Model_SetScreenYOverride(0x8000);gModel_bSceneMode=0;gModel_bPresentationPhase=-1;}
