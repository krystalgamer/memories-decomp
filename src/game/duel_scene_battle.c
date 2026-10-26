/*
 * Duel scene-state 9: the battle. It presents the attacker and the defender,
 * resolves the exchange, shows the damage and the guardian-star bonus, and
 * hands off to the trap, fusion-result and turn-switch states.
 *
 * Matches retail under gcc_2_8_1_g8_split_comm with no inline assembly,
 * register pins or asm links. What the listing fixes about the source:
 * - One s32 (result) carries the card stats, the battle result, the level
 *   nibble, the damage and the fade step: retail keeps all of them in $s2
 *   because the same variable crosses the sound call in state 9.
 * - The two saved stat words and the two saved flag words are side pairs
 *   (u16 [2]): as struct elements their loads cannot pass the record stores,
 *   which is why retail reloads gDuel_wSavedTargetRecordFlags after each flags store.
 * - State 4 falls through to its flag tests when there is no defender (the
 *   candidate returned there), and the do-while gives both exits one label so
 *   cse cannot thread the first test past the reload.
 * - State 3, 8, 9 and 10 store their next state inline; cross-jumping merges
 *   the identical tails and reorg lifts the li into the branch slot.
 * - Each state reads gDuel_bSceneSubStep into its own block-local variable; the
 *   function-wide copy conflicted with the temporaries retail keeps in $v1.
 * - The state-4 request block reads the flags before the field stores into a
 *   block-local, keeps the x coordinate and the level index in one shared
 *   variable so the second assignment waits for the first store, and the
 *   record pointer is &gCardPreview_Resources[gDuel_bPresentationCursor] with the meter copied through
 *   the sound-id scratch (retail's $a0).
 * - State 8 copies the position words through non-struct views so the copy
 *   cannot pass the flag byte store, and the shake block's flags are an s32
 *   so the mask writes the same register.
 * - State 5 writes the model properties in member order (model id first);
 *   the defender's stat word goes through a byte view so the pair loads for
 *   Duel_CompareBattleStrength wait for it.
 * - The two replay bytes are tentative scalar definitions, gDuel_bBattleRemovalSlot0 and
 *   gDuel_bBattleRemovalSlot1: only a definition in this unit makes gcc treat them as small
 *   data and emit the load-delay nops retail has before their gp-relative
 *   stores, and only two symbols let the stores keep retail's order without
 *   cse deriving the second address from the first. The attacker's card id
 *   is read through a byte view so it stays behind those stores. The _comm
 *   profile keeps the definitions common, so the linker resolves them to
 *   the retail addresses.
 */
#define D_8009B369_IN_DATA
#define D_8009B374_IN_DATA
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#define D_8009B170_AS_SIDE_ARRAY
#define D_8009B178_AS_SIDE_ARRAY
#include "../types.h"
#include "../psyq/rand.h"
#include "duel_scene_state.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_side_state.h"
#include "duel_action_lock.h"
#include "duel_init_scene.h"
#include "duel_card_staging.h"
#include "duel_card.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "duel_selection_layout.h"
#include "duel_card_pick_cursor.h"
#include "fade.h"
#include "main_mode_state.h"
#include "duel_effect_request.h"
#include "duel_effect_allocate_request.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_resource_setup.h"
#define D_8009B1B9_AS_SIGNED
#include "card_preview_objects.h"
#include "display_object_helpers.h"
#include "display_object_core.h"
#include "display_object_start_alpha_transition.h"
#include "duel_battle_stats.h"
#include "duel_trap_resolution.h"
#include "duel_card_record_lifecycle.h"
#include "duel_scene_battle.h"
#include "duel_scene_field_actions.h"
#define D_8009B208_AS_REPLAY_BYTES
#include "duel_scene_resume.h"
#include "duel_scene_card_placement.h"
#include "duel_presentation_controls.h"
#include "sound.h"
#include "main_modes.h"
#include "duel_card_capture_rounded_texture.h"
#include "duel_card_display_objects.h"
#include "sound_output.h"
#include "sound_sequence_state.h"
#include "model_scene_states.h"
#include "display_object_motion.h"
#include "../unmatched.h"
#include "fade_state_bytes.h"
#include "duel_session_controls.h"
#include "display_callback_transitions.h"
#include "duel_battle_removal_slots.h"
#include "duel_grid.h"
#include "duel_view_tween.h"

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))

/* Regional battle layouts override these two vertical coordinates. */
#ifndef DUEL_BATTLE_STAT_DISPLAY_Y
#define DUEL_BATTLE_STAT_DISPLAY_Y 0x16
#endif

#ifndef DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET
#define DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET 0x62
#endif

s8 gDuel_bBattleRemovalSlot0;
s8 gDuel_bBattleRemovalSlot1;

void DuelScene_UpdateBattle(void)
{
    DisplayObject *o;
    DisplayObject *side;
    DuelCardRecord *rec;
    DuelCardRecord *left;
    DuelCardRecord *right;
    DuelEffectRequest *req;
    s32 result;
    s32 id;
    s32 level;
    s32 arg;
    u16 life;
    u8 flags;
    s32 ix;
    s8 *wins;
    DuelSideState *view;
    AnimatedBattleModelProperties *models;
    DisplayObject *G32 *pair;
    DisplayObject *G32 *slots;
    DisplayObjectCallback cb;
    s32 one;
    s32 y;
    s32 h;
    s32 w;
    s32 big;
    s32 pos;
    DuelSelectionRecord *cur;
    u8 *pw;
    DuelEffectResourceRecord *effects;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        big = 0x48000;
        gDuel_wSceneStateFlags |= 0x8000;
        w = 0x10;
        one = 1;
        slots = gDisplayObject_apWorkSlots;
        pw = gDuel_abCardStagingBuffer;
        o = gDuel_pTerrainIcon;
        y = 0x5C;
        S(o, 0x28) = -0x74;
        cb = (DisplayObjectCallback)DisplayObject_UpdateCosineSlideWithRollReset;
        o->field_2C.h.field_2C = w;
        o->field_6C = one;
        o->update = cb;
        o->position.h.field_2A = o->field_30.h.field_32;
        o = gDuel_pLifePointsPanel;
        h = 0x18;
        o->position.h.field_28 = 0x198;
        o->field_2C.h.field_2C = w;
        o->field_6C = one;
        o->update = cb;
        o->position.h.field_2A = o->field_30.h.field_32;
        o = slots[0];
        o->position.h.field_28 = 0x38;
        o->position.h.field_2A = y;
        o->field_2C.h.field_2C = h;
        o->field_6C = one;
        o->update = cb;
        CardPreview_RequestResourceTransfer(0,
            ((DuelCardReplayRecordBlock *)(
                pw + gDisplayObject_apWorkSlots[0]->field_6A * 0x1C + big))->record.card_id);
        o = slots[1];
        gDuel_wSelectedTrapCardId = 0;
        gDuel_bSelectedTrapSlot = 0;
        if (o != 0) {
            o->position.h.field_28 = 0xD8;
            o->position.h.field_2A = y;
            o->field_2C.h.field_2C = h;
            o->field_6C = one;
            o->update = cb;
        } else {
            gDuel_bAnimatedBattleRequested = 0;
        }
        if (Duel_SelectAttackTrap(gDisplayObject_apWorkSlots[0]) != 0) {
            gDuel_bAnimatedBattleRequested = 0;
        }
        cur = (DuelSelectionRecord *)(gDuel_abSelectionRecord3 + gDuel_bActiveSide * 0x70);
        gDuel_pActiveSelectionRecord = cur;
        DisplayObject_ReleaseIfPresent(cur->cursor_object);
        gDuel_wDisplayTransitionFlags = 8;
        gDuel_bSceneSubStep = 1;
        gDuel_pActiveSelectionRecord->cursor_object = 0;
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        if (!(gDuel_bSceneSubStep & 0x80) && ((gFile_dwTransferState_abs & 0x02000030) | gFile_dwCancelRequestState_abs) == 0) {
            if (!(gDuel_bSceneSubStep & 0x40)) {
                id = gDuel_wSelectedTrapCardId;
                gDuel_bSceneSubStep |= 0x40;
                if (id == 0) {
                    if (gDisplayObject_apWorkSlots[1] != 0) {
                        u8 *pw1 = gDuel_abCardStagingBuffer;
                        s32 big1 = 0x48000;

                        id = ((DuelCardReplayRecordBlock *)(
                            pw1 + gDisplayObject_apWorkSlots[1]->field_6A * 0x1C + big1))->record.card_id;
                        goto request;
                    }
                } else {
                request:
                    CardPreview_RequestResourceTransfer(1, id);
                    return;
                }
            } else {
                gDuel_bSceneSubStep |= 0x80;
                goto wait;
            }
        } else {
        wait:
            if (!(gDuel_bSceneSubStep & 0x20) && gDuel_wDisplayTransitionFlags == 0) {
                if (!(gDuel_bSceneSubStep & 0x10)) {
                    gDuel_bSceneSubStep |= 0x10;
                    gDuel_wDisplayTransitionFlags = 0xC;
                    gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionRecord2 + gDuel_bActiveSide * 0x70);
                } else {
                    gDuel_bSceneSubStep |= 0x20;
                }
            }
            if ((gDuel_bSceneSubStep & 0xA0) == 0xA0 && DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep = 2;
                return;
            }
        }
        break;

    case 2: {
        u8 f = gDuel_bSceneSubStep;

        if (!(f & 0x80)) {
            o = gDisplayObject_apWorkSlots[0];
            gDuel_bSceneSubStep = f | 0x80;
            {
                s32 b21 = o->field_20.b.rotation_y;

                DisplayObject_StartAlphaTransition(o, b21, b21 + 0x40, 8);
            }
            result = Duel_CalcCardStats(&gDuel_aCardRecords[o->field_6A]);
            o = (DisplayObject *)CardPreview_CreateResourceObjects(0, result & 0xFFFF, result >> 16);
            effects = gCardPreview_Resources;
            effects[0].stat_palette_flags |= 0x40;
            o->field_30.h.field_30 = 0xA;
            o->field_30.h.field_32 = DUEL_BATTLE_STAT_DISPLAY_Y;
            o->field_20.b.rotation_y = 0xC0;
            DisplayObject_SetDepthOffset(o, -0xA);
            o->flags = (o->flags | 4) & 0xFFBF;
            gDisplayObject_apWorkSlots[2] = o;
            o = gDisplayObject_apWorkSlots[1];
            gDisplayObject_apWorkSlots[3] = 0;
            if (o != 0) {
                {
                    s32 b21 = o->field_20.b.rotation_y;

                    DisplayObject_StartAlphaTransition(o, b21, b21 + 0x40, 8);
                }
                result = Duel_CalcCardStats(&gDuel_aCardRecords[o->field_6A]);
                o = (DisplayObject *)CardPreview_CreateResourceObjects(1, result & 0xFFFF, result >> 16);
                if (gDuel_awSavedRecordFlags[1] & 0x800) {
                    effects[1].stat_palette_flags |= 0x80;
                } else {
                    effects[1].stat_palette_flags |= 0x40;
                }
            } else if (gDuel_wSelectedTrapCardId != 0) {
                o = (DisplayObject *)CardPreview_CreateResourceObjects(1, -1, -1);
            }
            if (o != 0) {
                o->field_30.h.field_30 = 0xAA;
                o->field_30.h.field_32 = DUEL_BATTLE_STAT_DISPLAY_Y;
                o->field_20.b.rotation_y = 0xC0;
                DisplayObject_SetDepthOffset(o, -0xA);
                o->flags = (o->flags | 4) & 0xFFBF;
                gDisplayObject_apWorkSlots[3] = o;
            }
        }
        f = gDuel_bSceneSubStep;
        if (f & 0x20) {
            if (Duel_UpdateTrapPresentation() == 0) {
                gDuel_bSceneSubStep &= 0xDF;
                return;
            }
        } else if (!(f & 0x40)) {
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDisplayObject_apWorkSlots[0]->flags &= 0xFFBF;
                gDisplayObject_apWorkSlots[2]->flags |= 0x40;
                if (gDisplayObject_apWorkSlots[1] != 0) {
                    gDisplayObject_apWorkSlots[1]->flags &= 0xFFBF;
                }
                if (gDisplayObject_apWorkSlots[3] != 0) {
                    gDisplayObject_apWorkSlots[3]->flags |= 0x40;
                }
                {
                    u8 g = gDuel_bSceneSubStep;

                    gDuel_bSceneSubStep = g | 0x40;
                    if (gDuel_wSelectedTrapCardId != 0) {
                        gDuel_bSceneSubStep = g | 0x60;
                    gDuel_wTrapPresentationMode = 0;
                    return;
                    }
                }
            }
        } else {
            gDisplayObject_apWorkSlots[2]->field_20.b.rotation_y += 8;
            if ((s8)gDisplayObject_apWorkSlots[2]->field_20.b.rotation_y >= 0) {
                gDisplayObject_apWorkSlots[2]->field_20.b.rotation_y = 0;
                gDuel_bSceneSubStep = 3;
                gDisplayObject_apWorkSlots[2]->flags &= 0xFFFB;
            }
            if (gDisplayObject_apWorkSlots[3] != 0) {
                u8 fade = gDisplayObject_apWorkSlots[2]->field_20.b.rotation_y;

                gDisplayObject_apWorkSlots[3]->field_20.b.rotation_y = fade;
                if (fade == 0) {
                    gDisplayObject_apWorkSlots[3]->flags &= 0xFFFB;
                    return;
                }
            }
        }
        break;
    }

    case 3:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            Fade_StartOutKeepOverlayAndHideSecondaryTables();
            return;
        }
        if (!(gFade_bFlags[0] & 0x80)) {
            gDuel_bSceneSubStep = 4;
            if (gDuel_wSelectedTrapCardId != 0) {
                gDuel_bPresentationCursor = gDuel_wSelectedTrapCardId == 0x2B2;
                gDuel_bSceneSubStep = 0xA;
                return;
            }
            result = Duel_CompareBattleStrength(gDisplayObject_apWorkSlots[0], gDisplayObject_apWorkSlots[1]);
            if (result >= 0) {
                gDuel_abBattleOutcome[0] = 0;
                gDuel_awBattleDamage[0] = 0;
                gDuel_abBattleOutcome[1] = 1;
                gDuel_awBattleDamage[1] = 0;
                if (result != 0) {
                    if (gDisplayObject_apWorkSlots[1] != 0 && (gDuel_awSavedRecordFlags[1] & 0x800)) {
                        gDuel_pActiveSideState->rank.field_0B++;
                        gDuel_abBattleOutcome[1] = -1;
                        gDuel_aSideStates[gDuel_bActiveSide ^ 1].rank.field_0C++;
                        return;
                    }
                    view = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
                    life = view->life_points.unsigned_value - result;
                    view->life_points.unsigned_value = life;
                    if ((s16)life < 0) {
                        view->life_points.unsigned_value = 0;
                    }
                    if (gDisplayObject_apWorkSlots[1] != 0) {
                        gDuel_pActiveSideState->rank.field_0B++;
                        gDuel_pActiveSideState->rank.effective_attacks++;
                    }
                    gDuel_abBattleOutcome[1] = -1;
                    gDuel_awBattleDamage[1] = result;
                    return;
                }
            } else {
                gDuel_abBattleOutcome[0] = -1;
                gDuel_abBattleOutcome[1] = -1;
                gDuel_awBattleDamage[0] = 0;
                gDuel_awBattleDamage[1] = 0;
                if (result < -1) {
                    view = &gDuel_aSideStates[gDuel_bActiveSide];
                    life = view->life_points.unsigned_value + result;
                    view->life_points.unsigned_value = life;
                    if ((s16)life < 0) {
                        view->life_points.unsigned_value = 0;
                    }
                    gDuel_awBattleDamage[0] = result;
                    gDuel_abBattleOutcome[1] = 1;
                    if (gDuel_awSavedRecordFlags[1] & 0x800) {
                        gDuel_abBattleOutcome[0] = 1;
                        gDuel_aSideStates[gDuel_bActiveSide ^ 1].rank.defensive_wins++;
                        return;
                    }
                }
            }
        }
        break;

    case 4:
        do {
            if (gDuel_bSceneSubStep & 0x80) {
                break;
            }
            gDuel_bSceneSubStep |= 0x80;
            if (gDisplayObject_apWorkSlots[1] == 0) {
                break;
            }
            {
                result = Duel_CalcGuardianStarBonus(&gDuel_aCardRecords[gDisplayObject_apWorkSlots[0]->field_6A],
                                                    &gDuel_aCardRecords[gDisplayObject_apWorkSlots[1]->field_6A]);
                if (result != 0) {
                    side = gDisplayObject_apWorkSlots[2];
                    gDuel_bPresentationCursor = 0;
                    if (result < 0) {
                        side = gDisplayObject_apWorkSlots[3];
                        gDuel_bPresentationCursor = 1;
                    }
                    rec = &gDuel_aCardRecords[gDisplayObject_apWorkSlots[gDuel_bPresentationCursor]->field_6A];
                    if (rec->flags & 0x200) {
                        level = gDuel_adwCardStats[rec->card_id - 1] >> 0x12;
                    } else {
                        level = gDuel_adwCardStats[rec->card_id - 1] >> 0x16;
                    }
                    result = level & 0xF;
                    SD_SEPlayFull(0x1D);
                    req =
                        DuelEffect_AllocateRequest(0xE);
                    {
                        u8 t = gDuel_bSceneSubStep;

                        gDuelEffect_pTrackedRequest = req;
                        pos = side->field_30.h.field_30;
                        pos += 0x46;
                        req->field_00 = pos;
                        gDuel_wStepCounter = 0;
                        arg = side->field_30.h.field_32;
                        arg += DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET;
                        req->field_02 = arg;
                        pos = result - 1;
                        req->phase = pos;
                        gDuel_bSceneSubStep = t | 0x60;
                    }
                }
                return;
            }
        } while (0);
        {
            u8 f = gDuel_bSceneSubStep;

            if (f & 0x20) {
                if (!(gDuelEffect_pTrackedRequest->flags & 0x80)) {
                    gDuel_bSceneSubStep = f & 0xDF;
                }
            }
            f = gDuel_bSceneSubStep;
            if (f & 0x40) {
                /* Separate from effects to preserve state 2's register setup. */
                DuelEffectResourceRecord *resource;

                gDuel_wStepCounter += 0x10;
                if ((s16)gDuel_wStepCounter >= 0x1F4) {
                    gDuel_bSceneSubStep = f & 0xBF;
                    gDuel_wStepCounter = 0x1F4;
                }
                resource = &gCardPreview_Resources[gDuel_bPresentationCursor];
                arg = gDuel_wStepCounter;
                *(u16 *)&resource->attack_adjustment = arg;
                *(u16 *)&resource->defense_adjustment = arg;
            }
            if (!(gDuel_bSceneSubStep & 0x60)) {
                gDuel_bSceneSubStep = 7;
                if (gDuel_bAnimatedBattleRequested != 0) {
                    gDuel_bSceneSubStep = 5;
                    return;
                }
            }
        }
        break;

    case 5:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            SD_BGMFadeOut();
            Fade_StartOut();
            gFade_abCurrentLevel[0] = 0xFF;
            Fade_FillBandLevels(0xFF);
            return;
        }
        if (!(gFade_bFlags[0] & 0x80) && SD_IsBgmFadeActive() == 0) {
            pair = gDisplayObject_apWorkSlots;
            SD_BGMPreload(gDuel_wAnimatedBattleBgmCommand);
            Model_SetPresentationBgmCommand(gDuel_wAnimatedBattleBgmCommand);
            left = &gDuel_aCardRecords[pair[0]->field_6A];
            right = &gDuel_aCardRecords[pair[1]->field_6A];
            Duel_SetupFieldCardFromDeck(pair[0]->field_6A, pair[0]->field_6B);
            left->flags |= (gDuel_awSavedRecordFlags[0] & 0xA00) | 0x4000;
            left->stat_modifier = gDuel_awSavedStatModifiers[0];
            Duel_SetupFieldCardFromDeck(pair[1]->field_6A, pair[1]->field_6B);
            right->flags |= gDuel_awSavedRecordFlags[1] & 0xA00;
            H(right, 0x12) = gDuel_awSavedStatModifiers[1];
            models = gMain_aAnimatedBattleModels;
            gDuel_bBattleRemovalSlot1 = -1;
            gDuel_bBattleRemovalSlot0 = -1;
            models[0].model_id = H(left, 0xC);
            models[0].defense_position = 0;
            models[0].field_02 = 0;
            models[0].field_04 = 0;
            models[0].guardian_star_selection = (gDuel_awSavedRecordFlags[0] >> 9) & 1;
            models[1].model_id = right->card_id;
            models[1].guardian_star_selection = (gDuel_awSavedRecordFlags[1] >> 9) & 1;
            models[1].field_02 = 0;
            models[1].field_04 = 0;
            models[1].defense_position = (gDuel_awSavedRecordFlags[1] >> 0xB) & 1;
            result = Duel_CompareBattleStrength(pair[0], pair[1]);
            if (result == -1) {
                gDuel_bBattleRemovalSlot0 = pair[0]->field_6A;
                gDuel_bBattleRemovalSlot1 = pair[1]->field_6A;
            } else {
                if (result > 0) {
                    models[0].field_02 = 1;
                    gDuel_bBattleRemovalSlot0 = pair[1]->field_6A;
                }
                if (result < 0) {
                    if (!(right->flags & 0x800)) {
                        models[1].field_02 = 1;
                        gDuel_bBattleRemovalSlot0 = pair[0]->field_6A;
                    }
                }
            }
            gDuel_bResumeFromAnimatedBattle = 1;
            gMain_bReturnMode = 3;
            gMain_bActiveModeState = 1;
            return;
        }
        break;

    case 7:
        if (gDisplayObject_apWorkSlots[1] == 0) {
            gDuel_bSceneSubStep = 9;
            return;
        }
        gDuel_bPresentationCursor = 2;
        gDuel_bSceneSubStep = 6;
    case 6:
        gDuel_bPresentationCursor = gDuel_bPresentationCursor - 1;
        if ((s8)gDuel_bPresentationCursor >= 0) {
            gDuel_bSceneSubStep = 8;
        case 8:
            flags = gDuel_bSceneSubStep;
            ix = gDuel_bPresentationCursor;
            side = gDisplayObject_apWorkSlots[ix + 2];
            if (!(flags & 0x80)) {
                gDuel_bSceneSubStep = flags | 0xC0;
                *(u32 *)&side->position = *(u32 *)&side->field_30;
                wins = gDuel_abBattleOutcome;
                if (wins[ix] != 0) {
                    req =
                        DuelEffect_AllocateRequest(2);
                    req->field_00 = side->field_30.h.field_30 + 0x46;
                    req->field_02 =
                        side->field_30.h.field_32 +
                        DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET;
                    result = gDuel_awBattleDamage[gDuel_bPresentationCursor];
                    gDuelEffect_pTrackedRequest = req;
                    req->field_12 = result;
                    result = (result < 0 ? -result : result) / 1000;
                    if (result >= 3) {
                        result = 2;
                    }
                    req->phase = result;
                    if (wins[gDuel_bPresentationCursor] > 0) {
                        gDuel_bSceneSubStep |= 0x20;
                    }
                    arg = result + 0x10;
                    if (gDuel_bPresentationCursor == 0) {
                        arg = result + 0xD;
                    }
                    SD_SEPlayFull(arg);
                    goto shake;
                }
                gDuel_bSceneSubStep = 6;
                return;
            }
        shake:
        {
            s32 f = gDuel_bSceneSubStep;

            if (f & 0x40) {
                if (!(gDuelEffect_pTrackedRequest->flags & 0x80)) {
                    f &= 0xBF;
                    gDuel_bSceneSubStep = f;
                    *(u32 *)&side->field_30 = side->position.word;
                    gDuel_wStepCounter = 0xA;
                    return;
                }
                if (!(f & 0x20) &&
                    gDuelEffect_pTrackedRequest->progress != 0) {
                    side->field_30.h.field_30 = side->position.h.field_28 + ((rand() & 3) - 2);
                    side->field_30.h.field_32 = side->position.h.field_2A + ((rand() & 3) - 2);
                    return;
                }
            } else {
                gDuel_wStepCounter = gDuel_wStepCounter - 1;
                if ((s16)gDuel_wStepCounter <= 0) {
                    if (gDuel_bSceneSubStep & 0x20) {
                        gDuel_bSceneSubStep = 6;
                        return;
                    }
                    gDuel_bSceneSubStep = 0xA;
                    return;
                }
            }
        }
        } else {
            gDuel_bSceneSubStep = 0xB;
            return;
        }
        break;

    case 9:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            req = DuelEffect_CreateRequest(2);
            req->field_00 = 0xF0;
            req->field_02 = 0x78;
            result = Duel_CompareBattleStrength(gDisplayObject_apWorkSlots[0], 0);
            req->phase = result / 1000;
            if (req->phase >= 3) {
                req->phase = 2;
            }
            SD_SEPlayFull(req->phase + 0x10);
            req->field_12 = result;
            req->phase = req->phase + 3;
            return;
        }
        gDuel_bSceneSubStep = 0xB;
        return;

    case 10:
        side = gDisplayObject_apWorkSlots[gDuel_bPresentationCursor + 2];
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            DuelCard_CaptureRoundedTexture(side);
            SD_SEPlayFull(0x1B);
            req = DuelEffect_AllocateRequest(3);
            gDuelEffect_pTrackedRequest = req;
            req->field_00 = side->field_30.h.field_30 + 0x46;
            req->field_02 =
                side->field_30.h.field_32 +
                DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET;
            if (gDuel_wSelectedTrapCardId != 0) {
                req->phase = 1;
            }
            gDuel_wStepCounter = 2;
            return;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            gDuel_wStepCounter = gDuel_wStepCounter - 1;
            if ((s16)gDuel_wStepCounter <= 0) {
                gDuel_bSceneSubStep |= 0x40;
                DuelEffect_ReleaseResourceObjects(gDuel_bPresentationCursor);
                if (gDuel_bPresentationCursor != 1 || gDuel_wSelectedTrapCardId == 0) {
                    gDisplayObject_apWorkSlots[gDuel_bPresentationCursor + 2] = 0;
                    return;
                }
            }
        } else if (!(gDuelEffect_pTrackedRequest->flags & 0x80)) {
            gDuel_bSceneSubStep = 6;
            if (gDuel_wSelectedTrapCardId != 0) {
                gDuel_bSceneSubStep = 0xB;
                return;
            }
        }
        break;

    case 11:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            if (gDisplayObject_apWorkSlots[2] != 0) {
                Duel_SetupFieldCardFromDeck(gDisplayObject_apWorkSlots[0]->field_6A, gDisplayObject_apWorkSlots[0]->field_6B);
                left = &gDuel_aCardRecords[gDisplayObject_apWorkSlots[0]->field_6A];
                left->flags |= (gDuel_awSavedRecordFlags[0] & 0xA00) | 0x4000;
                left->stat_modifier = gDuel_awSavedStatModifiers[0];
                Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
            }
            if (gDisplayObject_apWorkSlots[3] != 0) {
                if (gDisplayObject_apWorkSlots[1] != 0) {
                    Duel_SetupFieldCardFromDeck(gDisplayObject_apWorkSlots[1]->field_6A, gDisplayObject_apWorkSlots[1]->field_6B);
                    left = &gDuel_aCardRecords[gDisplayObject_apWorkSlots[1]->field_6A];
                    left->flags |= gDuel_awSavedRecordFlags[1] & 0xA00;
                    left->stat_modifier = gDuel_awSavedStatModifiers[1];
                    if (gDuel_wSelectedTrapCardId != 0) {
                        left->flags |= gDuel_awSavedRecordFlags[1] & 0x3000;
                    }
                    Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
                }
            }
            DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots[0]);
            DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots[1]);
            o = gDuel_pTerrainIcon;
            o->position.h.field_28 = 0xC;
            o->field_2C.h.field_2C = 0x10;
            o->field_6C = 1;
            o->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlideWithRollReset;
            o->position.h.field_2A = o->field_30.h.field_32;
            o = gDuel_pLifePointsPanel;
            o->position.h.field_28 = 0x118;
            o->field_2C.h.field_2C = 0x10;
            o->field_6C = 1;
            o->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlideWithRollReset;
            o->position.h.field_2A = o->field_30.h.field_32;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            o = gDisplayObject_apWorkSlots[2];
            if (o != 0) {
                result = B(o, 0xC);
                result -= 8;
                if (result <= 0) {
                    DuelEffect_ReleaseResourceObjects(0);
                    gDisplayObject_apWorkSlots[2] = 0;
                } else {
                    B(o, 0xE) = result;
                    B(o, 0xD) = result;
                    B(o, 0xC) = result;
                }
            }
            o = gDisplayObject_apWorkSlots[3];
            if (o != 0) {
                result = B(o, 0xC);
                result -= 8;
                if (result <= 0) {
                    DuelEffect_ReleaseResourceObjects(1);
                    gDisplayObject_apWorkSlots[3] = 0;
                } else {
                    B(o, 0xE) = result;
                    B(o, 0xD) = result;
                    B(o, 0xC) = result;
                }
            }
            if (gDisplayObject_apWorkSlots[2] == 0 && gDisplayObject_apWorkSlots[3] == 0) {
                gDuel_bSceneSubStep |= 0x40;
                Fade_StartInKeepOverlay();
                return;
            }
        } else {
            gDuel_wSceneStateFlags = 5;
        }
        break;
    }
}

void DuelScene_UpdateTurnSwitch(void){if(!(gDuel_wSceneStateFlags&DUEL_SCENE_FLAG_INITIALIZED)){gDuel_wSceneStateFlags|=DUEL_SCENE_FLAG_INITIALIZED;Duel_StartViewTween(0x30,0x258,0x100,gDuel_bActiveSide?0x400:0xC00,0);gDuel_wDisplayTransitionFlags=0x10;}
 if(!gDuel_wDisplayTransitionFlags){gDuel_bActiveSide^=1;gDuel_wSceneStateFlags=2;gDuel_pActiveSideState=&gDuel_aSideStates[gDuel_bActiveSide];gDuel_pActiveFieldGrid=gDuel_abFieldGridCardRecordIndices+gDuel_bActiveSide*DUEL_FIELD_SIDE_GRID_SLOT_COUNT;}}
