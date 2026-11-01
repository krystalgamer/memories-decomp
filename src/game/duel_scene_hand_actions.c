/*
 * Duel scene-state 4: the player's hand. It builds the hand cursor, runs the
 * scripted selection when the side is computer controlled, lets the player
 * walk the hand, inspect, flip and mark cards, and hands the marked cards to
 * the placement state in the order they were picked.
 *
 * Matches retail under gcc_2_8_1_g8_split. What the listing fixes about the
 * source:
 * - The pad words are volatile: every test re-reads them.
 * - The AI selection bytes are read as elements of gAi_Selection.bytes ([1], [6] and
 *   [8]). spimdisasm names those addresses D_800EAE89, D_800EAE8E and
 *   gAi_bHandFaceDownRequested, but the symbol-plus-offset form is what puts the %hi ahead of
 *   the neighbouring constants, and the linked bytes are the same.
 * - The substate word is a scalar. sched lets a struct-member load through a
 *   pointer pass a store to a fixed scalar, which is the order of every
 *   "substate, then object flags" pair here; D_8009B20C[0] as an array element
 *   would hold those loads back.
 * - The object's position pairs are copied as whole words. The copy that waits
 *   behind the substate store is a plain word access; the one that moves ahead
 *   of the state store goes through the union member.
 * - One variable serves the scripted card id, the cursor step, the card kind,
 *   the viewer request, the grid id and the pick-order counter; it is the
 *   $s0 of every block. hand is reused to walk the table when a pick is
 *   withdrawn, and obj holds the cursor sprite while the state is set up.
 * - The pick-order scan is a one-pass do/while around a for (;;) whose first
 *   statement is the hit test. The hit block rotates to the loop end and is
 *   copied in front of the loop, where the slot index folds to zero, and the
 *   enclosing pass keeps loop.c from moving the miss block out of the loop.
 *   Duel_ApplyAiHandSelection next door has the same one-pass do/while.
 */
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define GDUEL_BEFFECTSTATE_IN_DATA
#define GDUEL_BCARDVIEWERYOFFSET_IN_DATA
#define GDUEL_WVIEWERCARDID_IN_DATA
#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define D_8009B360_AS_SIDE_ARRAY
#define FUNC_80024088_WIDE_DIRECTION
#include "../types.h"
#include "../psyq/stdio.h"
#include "duel_scene_hand_actions.h"
#include "duel_scene_state.h"
#include "duel_scene_resume.h"
#include "duel_side_state.h"
#include "duel_selection_layout.h"
#include "duel_field_selection.h"
#include "duel_hand_selection_colors.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_card.h"
#include "duel_deck_card.h"
#include "duel_card_staging.h"
#include "duel_card_display_state.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_pick_cursor.h"
#include "duel_cursor_status.h"
#include "duel_field_display_objects.h"
#include "duel_grid.h"
#include "duel_action_lock.h"
#include "duel_card_viewer.h"
#include "duel_effect.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "display_object_interpolation.h"
#include "display_object_motion.h"
#include "display_object_work_slots.h"
#include "duel_hand_position_selection_cursor.h"
#include "duel_hand_selection_cursor.h"
#include "duel_get_card_viewer_request_id.h"
#include "duel_check_quit_input.h"
#include "input.h"
#include "ai.h"
#include "ai_controller_handoff.h"
#include "script_state.h"
#include "sound.h"
#include "ai_board_snapshot.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "duel_card_defense_rotation.h"
#define gDuel_wActionSubstate SUBSTATE
#define SUBSTATE (*(u16 *)D_8009B20C)
#define SIDE_CURSOR(base) \
    ((DuelSelectionRecord *)((base) + gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE))
#define HAND_CARD_INDEX(side) \
    (gDuel_abHandCardRecordIndices[(s8)(side)->field_0E + gDuel_bActiveSide * 5])
#define FIELD_CURSOR \
    (((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2])
#define CARD_RECORD(i) (((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->field_cards[i])
#define CARD_ID(i) (CARD_RECORD(i).card_id)
#define CARD_KIND(id) \
    ((gDuel_adwCardStats[(id) - 1] >> CARD_STAT_TYPE_SHIFT) & \
     CARD_STAT_TYPE_MASK)
#define HAND_CARD_OBJECT_VIEW(object) ((HandCardObject *)(object))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
/* The two choice sprites set their flags through the halfword, not the
 * record: the store stays ahead of the pointer's own store. */
#define SPRITE_FLAGS(sprite) (*(u16 *)((u8 *)(sprite) + 8))

#ifndef DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION
#define DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION 1
#endif

#ifdef VERSION_JAPAN
/* The Japanese build cancels with Cross, confirms with Circle or Square, and
 * reads the card viewer's cell from the side's own column and row. */
#define HAND_ACTIONS_CANCEL_MASK PAD_BUTTON_CROSS
#define HAND_ACTIONS_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define HAND_ACTIONS_VIEWER_CELL (side->row * 5 + side->col)
#else
#define HAND_ACTIONS_CANCEL_MASK PAD_BUTTON_CANCEL
#define HAND_ACTIONS_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#define HAND_ACTIONS_VIEWER_CELL (FIELD_CURSOR.row * 5 + FIELD_CURSOR.col)
#endif

void DuelScene_UpdateHandActions(void)
{
    DuelSelectionRecord *side;
    DuelHandSlot *hand;
    DuelHandSlot *slot;
    DuelHandSlot *first_slot;
    DisplayObject *G32 *out;
    u8 *sprite;
    HandCardObject *obj;
    DuelCardRecord *card;
    s32 n;
    s32 i;
    s32 v;
    s32 value;
    s32 card_id;

    side = (DuelSelectionRecord *)(gDuel_abSelectionSides + gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        for (n = 6; n >= 0; n--) {
            gDisplayObject_apWorkSlots[n] = 0;
        }
        gDuel_pFlipRightMarker = 0;
        gDuel_pFlipLeftMarker = 0;
        gDuel_pActiveSelectionRecord = side;
        gDuel_wDisplayTransitionFlags = 0;
        side->field_0E = 0;
        side->field_15 = 0;
        obj = HAND_CARD_OBJECT_VIEW(
            DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2));
        DisplayObject_ConfigureSpriteResource(
            DISPLAY_OBJECT_VIEW(obj), 3, 0, 2, 0xB, 0x20C);
        obj->flags |= 0x28;
        DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(obj));
        DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(obj), 0xA);
        side->cursor_object = DISPLAY_OBJECT_VIEW(obj);
        DuelHand_PositionSelectionCursor(side);
        DuelField_CreateCardInfoTextBox(side, HAND_CARD_INDEX(side));
        gDuel_bSceneSubStep = 1;
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0) {
            gDuel_bSceneSubStep = 2;
        }
        return;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        Duel_UpdateCardPickCursor(gDuel_pActiveSelectionRecord);
        if (gDuelCursor_PickFlags != 0) {
            return;
        }
        gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides);
        gDuel_wSceneStateFlags &= 0xBFFF;
        return;
    }

    hand = &gDuelHand_aSlots[(s8)side->field_0E];
    switch (gDuel_bSceneSubStep & 0xF) {
    case 2:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wActionSubstate = 0;
        }
        switch (gDuel_wActionSubstate & 0xF) {
        case 0:
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0x8000;
                Ai_RefreshBoardSnapshot();
                AiScript_Init(D_801A8000);
                return;
            }
            if (AiScript_Run() != 0) {
                *(s8 *)&gDuel_bAiSelectionCursor = -1;
                gDuel_wAiHandStackPending = 0;
                if (gAi_Selection.bytes[1] != 0) {
                    gDuel_wAiHandStackPending = 1;
                }
                gDuel_wActionSubstate = 1;
                Duel_ApplyAiHandSelection();
                return;
            }
            return;
        case 1:
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0x8000;
                gDuel_bAiSelectionCursor++;
                value = gAi_Selection.bytes[(s8)gDuel_bAiSelectionCursor];
                if (value == 0) {
                    DuelHand_RefreshSelectionColors(side);
                    gDuel_wActionSubstate = 2;
                    gDuel_wAiHandStackPending &= 1;
                    if (gDuel_wAiHandStackPending != 0) {
                        gDuel_wActionSubstate = 3;
                    }
                    SD_SEPlayFull(7);
                    return;
                }
                if (value < 0xB) {
#if DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION
                    printf(gDuel_szComSelCardError);
#endif
                    return;
                }
                D_8009B208[2] = value - 0xB;
                gDuel_wAiSelectionStepDelay = 6;
            }
            gDuel_wAiSelectionStepDelay--;
            if ((s16)gDuel_wAiSelectionStepDelay <= 0) {
                gDuel_wAiSelectionStepDelay = 6;
                if ((s8)side->field_0E != D_8009B208[2]) {
                    SD_SEPlayFull(6);
                    if ((s8)side->field_0E > D_8009B208[2]) {
                        side->field_0E -= 1;
                    } else {
                        side->field_0E += 1;
                    }
                    DuelHand_PositionSelectionCursor(side);
                    DuelField_CreateCardInfoTextBox(side, HAND_CARD_INDEX(side));
                    return;
                }
                if (gDuel_wAiHandStackPending & 1) {
                    DuelHand_CreateSelectionOrderMarker(side);
                }
                gDuel_wActionSubstate &= 0x7FFF;
                return;
            }
            return;
        case 2:
            obj = HAND_CARD_OBJECT_VIEW(hand->object);
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0xC000;
                card_id = CARD_ID(obj->card_index);
                if (CARD_KIND(card_id) < CARD_TYPE_MAGIC) {
                    if (gAi_Selection.bytes[8] == 0) {
                        gDuel_wActionSubstate = 3;
                        return;
                    }
                }
                obj->target.xy.x = 0x86;
                obj->target.xy.y = 0x5A;
                obj->saved.xy.x = 0x10;
                obj->field_6C = 1;
                obj->update = DisplayObject_UpdateCosineSlide;
                DisplayObject_SetDepthOffset(
                    DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth + 4));
                side->cursor_object->flags &= 0xFFBF;
                return;
            }
            if (gDuel_wActionSubstate & 0x4000) {
                if (DisplayObject_FindAllocatedByTag(1) == 0) {
                    gDuel_wActionSubstate &= 0xBFFF;
                    if (gDuel_wActionSubstate & 0x1000) {
                        DisplayObject_SetDepthOffset(
                            DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth - 4));
                        gDuel_wActionSubstate = 3;
                        side->cursor_object->flags |= 0x40;
                        return;
                    }
                    obj->saved.xy.x = obj->home_x;
                    obj->saved.xy.y = obj->home_y;
                    if (gAi_Selection.bytes[8] != 0) {
                        SD_SEPlayFull(0xB);
                        gDuel_wActionSubstate |= 0x2000;
                        obj->flags |= 4;
                        return;
                    }
                }
            } else if (gDuel_wActionSubstate & 0x2000) {
                obj->face += 8;
                if ((s8)obj->face < 0) {
                    obj->face = 0x80;
                    gDuel_aCardRecords[obj->card_index].flags |= 0x1000;
                    gDuel_wActionSubstate &= 0xDFFF;
                    return;
                }
            } else {
                card_id = CARD_ID(obj->card_index);
                value = CARD_KIND(card_id);
                if (value >= CARD_TYPE_MAGIC && obj->face == 0 &&
                    value != CARD_TYPE_EQUIP && value != CARD_TYPE_TRAP) {
                    gDisplayObject_apWorkSlots[0] = DISPLAY_OBJECT_VIEW(obj);
                    gDuel_pActiveSideState->hand[(s8)side->field_0E] = -1;
                    gDuel_bSceneSubStep = 5;
                    return;
                }
                obj->update = DisplayObject_UpdateCosineSlide;
                gDuel_wActionSubstate |= 0x5000;
                *(s32 *)&obj->target = *(s32 *)&obj->saved;
                obj->saved.xy.x = 0x10;
                obj->field_6C = 1;
                return;
            }
            return;
        case 3:
            if (gDuel_wDisplayTransitionFlags != 0) {
                gDuelCursor_Direction = -1;
                return;
            }
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0xC000;
                Duel_BeginFieldPlacementSelection(side);
                D_8009B208[2] = gAi_Selection.bytes[6] - 6;
                if (D_8009B208[2] < 0) {
                    D_8009B208[2] = gAi_Selection.bytes[6] - 1;
                }
                gDuelCursor_Direction = -1;
                return;
            }
            if (DuelCursor_UpdateDirection(gDuel_pActiveSelectionRecord, gDuelCursor_Direction) == 0) {
                if (gDuel_pActiveSelectionRecord->col == D_8009B208[2]) {
                    goto confirm;
                }
                gDuelCursor_Direction = 0;
                if (D_8009B208[2] < gDuel_pActiveSelectionRecord->col) {
                    gDuelCursor_Direction = 2;
                }
            }
            return;
        }
        break;
    case 1:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            for (n = 0; n < 5; n++) {
                HAND_CARD_OBJECT_VIEW(gDuelHand_aSlots[n].object)->color = 0x808080;
            }
        }
        if (Duel_CheckQuitInput() != 0) {
            break;
        }
        if (gInput_wPad1Pressed & 0x10) {
            card_id = CARD_ID(HAND_CARD_INDEX(side));
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_bEffectState = 2;
            gDuel_wViewerCardID = card_id;
            return;
        }
        if (gInput_wPad1Repeat & 0xA000) {
            value = (s8)side->field_0E;
            if (gInput_wPad1Repeat & 0x2000) {
                value++;
                if (value >= 5) {
                    return;
                }
            } else {
                value--;
                if (value < 0) {
                    return;
                }
            }
            side->field_0E = value;
            DuelHand_PositionSelectionCursor(side);
            DuelField_CreateCardInfoTextBox(side, HAND_CARD_INDEX(side));
            SD_SEPlayFull(6);
            return;
        }
        if ((gInput_wPad1Pressed & 0xFFFF) == 0x1000 && gInput_wPad1Held == 0x1000) {
            if (hand->selection_order == 0) {
                DuelHand_CreateSelectionOrderMarker(side);
                return;
            }
        } else if ((gInput_wPad1Pressed & HAND_ACTIONS_CANCEL_MASK) ||
                   ((gInput_wPad1Pressed & 0xFFFF) == 0x4000 &&
                    gInput_wPad1Held == 0x4000)) {
            value = hand->selection_order;
            if (value != 0) {
                SD_SEPlayFull(0x2F);
                hand->selection_order = 0;
                HAND_CARD_OBJECT_VIEW(hand->object)->pos.xy.y += 4;
                DisplayObject_ReleaseIfPresent(hand->order_marker);
                hand->order_marker = 0;
                side->field_15--;
                hand = gDuelHand_aSlots;
                for (n = 0; n < 5; n++, hand++) {
                    v = hand->selection_order;
                    if (v >= value) {
                        hand->selection_order = v - 1;
                        HAND_CARD_OBJECT_VIEW(hand->order_marker)->icon_state =
                            (v - 2) * 0x10;
                    }
                }
                return;
            }
        } else {
            if (gInput_wPad1Held & 3) {
                gDuelCursor_PickFlags = 0;
                gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionRecord1);
                gDuel_wSceneStateFlags |= 0x4000;
                return;
            }
            if ((gInput_wPad1Pressed & HAND_ACTIONS_CONFIRM_MASK) && side->field_15 != 1) {
                SD_SEPlayFull(7);
                gDuel_bSceneSubStep = 4;
                DuelHand_RefreshSelectionColors(side);
                if (side->field_15 == 0) {
                    gDuel_bSceneSubStep = 3;
                    return;
                }
            }
        }
        break;
    case 3:
        obj = HAND_CARD_OBJECT_VIEW(hand->object);
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0xC0;
            obj->target.xy.x = 0x86;
            obj->target.xy.y = 0x5A;
            obj->saved.xy.x = obj->pos.xy.x;
            obj->saved.xy.y = obj->pos.xy.y;
            DisplayObject_SavePosition((void *)obj);
            obj->step = 0;
            DisplayObject_SetDepthOffset(
                DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth + 4));
            side->cursor_object->flags &= 0xFFBF;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            DisplayObject_InterpolatePositionCosine(
                (void *)obj, obj->target.xy.x, obj->target.xy.y, obj->step);
            obj->step = obj->step + 0xAA;
            if (obj->step >= 0x800) {
                gDuel_bSceneSubStep &= 0xBF;
                obj->pos.word = obj->target.word;
                if (gDuel_bSceneSubStep & 0x10) {
                    side->cursor_object->flags |= 0x40;
                    DisplayObject_SetDepthOffset(
                        DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth - 4));
                    if (gDuel_bSceneSubStep & 0x20) {
                        gDuel_bSceneSubStep = 4;
                        return;
                    }
                    gDuel_bSceneSubStep = 1;
                    return;
                }
                card_id = ((DuelDeckCardRecord *)CARD_RECORD(obj->card_index).data)->id;
                value = CARD_KIND(card_id);
                if ((value < CARD_TYPE_MAGIC || value == CARD_TYPE_TRAP) && obj->face == 0) {
                    obj->step = 0x10;
                    gDuel_bSceneSubStep |= 0x20;
                    SD_SEPlayFull(0xB);
                }
                sprite = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(
                    sprite, (s16)obj->pos.xy.x - 8, (s16)obj->pos.xy.y + 0x1E,
                    3, 1, 2, 0xB, 0x20C);
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(sprite));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(sprite), 0xA);
                SPRITE_FLAGS(sprite) |= 0x28;
                gDuel_pFlipLeftMarker = DISPLAY_OBJECT_VIEW(sprite);
                sprite = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(
                    sprite, (s16)obj->pos.xy.x + 0x3C, (s16)obj->pos.xy.y + 0x1E,
                    3, 1, 0, 0xB, 0x20C);
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(sprite));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(sprite), 0xA);
                SPRITE_FLAGS(sprite) |= 0x28;
                gDuel_pFlipRightMarker = DISPLAY_OBJECT_VIEW(sprite);
                return;
            }
        } else if (gDuel_bSceneSubStep & 0x20) {
            obj->flags |= 4;
            obj->face += obj->step;
            if (!(obj->face & 0x7F)) {
                if (obj->face == 0) {
                    obj->flags &= 0xFFFB;
                }
                gDuel_bSceneSubStep &= 0xDF;
                if (gDuel_bSceneSubStep & 0x10) {
                restore:
                    DisplayObject_ReleaseIfPresent(gDuel_pFlipLeftMarker);
                    DisplayObject_ReleaseIfPresent(gDuel_pFlipRightMarker);
                    gDuel_pFlipRightMarker = 0;
                    gDuel_pFlipLeftMarker = 0;
                    *(s32 *)&obj->target = *(s32 *)&obj->saved;
                    DisplayObject_SavePosition((void *)obj);
                    obj->step = 0;
                    gDuel_bSceneSubStep |= 0x50;
                    return;
                }
            }
        } else {
            if (gInput_wPad1Repeat & 0xA000) {
                obj->step = 0x10;
                gDuel_bSceneSubStep |= 0x20;
                if (gInput_wPad1Repeat & 0x2000) {
                    obj->step = -0x10;
                }
                SD_SEPlayFull(0xB);
                return;
            }
            if (gInput_wPad1Pressed & HAND_ACTIONS_CANCEL_MASK) {
                SD_SEPlayFull(8);
                if (obj->face != 0) {
                    obj->step = 0x10;
                    gDuel_bSceneSubStep |= 0x30;
                    return;
                }
                goto restore;
            }
            if (gInput_wPad1Pressed & HAND_ACTIONS_CONFIRM_MASK) {
                SD_SEPlayFull(7);
                gDuel_aCardRecords[obj->card_index].flags &= 0xEFFF;
                if (obj->face != 0) {
                    gDuel_aCardRecords[obj->card_index].flags |= 0x1000;
                }
                card_id = CARD_ID(obj->card_index);
                value = CARD_KIND(card_id);
                if (value >= CARD_TYPE_MAGIC && obj->face == 0 &&
                    value != CARD_TYPE_EQUIP && value != CARD_TYPE_TRAP) {
                    DisplayObject_ReleaseIfPresent(gDuel_pFlipLeftMarker);
                    DisplayObject_ReleaseIfPresent(gDuel_pFlipRightMarker);
                    gDisplayObject_apWorkSlots[0] = DISPLAY_OBJECT_VIEW(obj);
                    gDuel_pFlipRightMarker = 0;
                    gDuel_pFlipLeftMarker = 0;
                    gDuel_pActiveSideState->hand[(s8)side->field_0E] = -1;
                    gDuel_bSceneSubStep = 5;
                    return;
                }
                gDuel_bSceneSubStep |= 0x70;
                goto restore;
            }
        }
        break;
    case 4:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wTrapPresentationMode = 0;
            Duel_BeginFieldPlacementSelection(side);
            gDuel_bSceneSubStep |= 0x40;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (gDuel_wDisplayTransitionFlags == 0) {
                gDuel_bSceneSubStep &= 0xBF;
                if (gDuel_bSceneSubStep & 0x10) {
                    gDuel_bSceneSubStep = 1;
                    gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides);
                    if (side->field_15 == 0) {
                        gDuel_bSceneSubStep = 3;
                        return;
                    }
                }
            }
        } else if (DuelCursor_UpdateFromInput(gDuel_pActiveSelectionRecord) == 0) {
            value = Duel_GetCardViewerRequestId(
                &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[HAND_ACTIONS_VIEWER_CELL +
                                       gDuel_bActiveSide * 20]]);
            if (value != 0) {
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_wViewerCardID = value;
                gDuel_bEffectState = 2;
                return;
            }
            if (gInput_wPad1Pressed & HAND_ACTIONS_CANCEL_MASK) {
                gDuel_wDisplayTransitionFlags = 0xC;
                gDuel_bSceneSubStep |= 0x50;
                SD_SEPlayFull(8);
                return;
            }
            if (gInput_wPad1Pressed & HAND_ACTIONS_CONFIRM_MASK) {
            confirm:
                SD_SEPlayFull(7);
                for (n = 6; n >= 0; n--) {
                    gDisplayObject_apWorkSlots[n] = 0;
                }
                n = gDuel_pActiveSelectionRecord->row * 5 + gDuel_pActiveSelectionRecord->col;
                value = gDuel_abFieldGridCardRecordIndices[n + gDuel_bActiveSide * 20];
                card = &gDuel_aCardRecords[value];
                gDuel_bCardMoveSlot = value;
                i = 0;
                if (card->flags & 0x8000) {
                    obj = HAND_CARD_OBJECT_VIEW(card->object);
                    i = 1;
                    gDisplayObject_apWorkSlots[0] = DISPLAY_OBJECT_VIEW(
                        Duel_CreateCardDisplayObject(
                            card, card->screen_x, card->screen_y));
                    DuelCard_DeactivateRecord(&gDuel_aCardRecords[obj->card_index]);
                } else if (side->field_15 == 0 &&
                           HAND_CARD_OBJECT_VIEW(hand->object)->kind ==
                               CARD_TYPE_EQUIP &&
                           gDuel_pActiveSelectionRecord->row == 2) {
                    return;
                }
                if (side->field_15 == 0) {
                    hand->selection_order = 1;
                    if (HAND_CARD_OBJECT_VIEW(hand->object)->face != 0) {
                        gDuel_pActiveSideState->rank.face_down_plays++;
                    }
                }
                /* Collect the marked cards in pick order. A missing order
                 * ends the walk: 8 is past the loop bound. */
                for (value = 1, first_slot = gDuelHand_aSlots; value < 6; value++) {
                    s8 none = -1;

                    do {
                        n = 0;
                        out = &gDisplayObject_apWorkSlots[i];
                        slot = first_slot;
                        for (;;) {
                            if (slot->selection_order == value) {
                                gDuel_pActiveSideState->hand[n] = none;
                                i++;
                                *out = slot->object;
                                break;
                            }
                            n++;
                            slot++;
                            if (n >= 5) {
                                value = 8;
                                break;
                            }
                        }
                    } while (0);
                }
                gDuel_bSceneSubStep = 6;
                return;
            }
        }
        break;
    case 6:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_wDisplayTransitionFlags = 0xC;
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionRecord2);
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 7;
        }
        break;
    case 5:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            hand->object = 0;
            gDuel_wDisplayTransitionFlags = 8;
            gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides);
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 6;
        }
        break;
    }
}

#undef gDuel_wActionSubstate
#undef SIDE_CURSOR
#undef HAND_CARD_INDEX
#undef FIELD_CURSOR
#undef CARD_RECORD
#undef CARD_ID
#undef CARD_KIND
#undef SPRITE_FLAGS

void DuelCard_UpdateDefenseRotation(DisplayObject *o)
{
    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o)) {
        o->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if (!o->field_20.b.rotation_z) {
            *(s16 *)&o->position.h.field_28 = -8;
            o->position.h.field_2A = 0xC0;
        } else {
            o->position.h.field_28 = 8;
            o->position.h.field_2A = 0;
        }
    }
    o->field_20.b.rotation_z += (s16)o->position.h.field_28;
    if (o->field_20.b.rotation_z < 0xC0) {
        DuelCardRecord *e = &gDuel_aCardRecords[o->field_6A];

        e->flags |= DUEL_CARD_FLAG_DEFENSE_POSITION;
        o->field_20.b.rotation_z = (s16)o->position.h.field_2A;
        if (!o->field_20.b.rotation_z) {
            e->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
            if (!o->field_20.b.rotation_y) {
                o->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            }
        }
        o->field_6C = 0;
        o->update = 0;
    }
}
