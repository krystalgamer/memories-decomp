/*
 * Duel scene-state 5: the field. It runs the scripted field path for a
 * computer-controlled side, moves the field cursor, opens the card viewer,
 * picks the attacker and its target, commits a fusion or sacrifice selection
 * and fades the field for the battle state.
 *
 * Matches retail under gcc_2_8_1_g8_split. What the listing fixes about the
 * source:
 * - The pad word is volatile: every test re-reads it.
 * - The side records are the DuelSelectionRecord struct, not byte views:
 *   the cursor-object loads pass the state stores only as struct members.
 *   Record 3 of the current side is gDuel_abSelectionSides + 0x54 (spimdisasm names it
 *   gDuel_abSelectionRecord3) so cse can derive one from the other, and the pick cursor is
 *   record 2 of the same table.
 * - The slot index row * 5 + col is a separate statement into the loop
 *   counter i wherever retail computes the sum into its own register.
 * - One variable holds the scripted card id, the viewer request, the attack
 *   row, the commit flag, the fade level and the sprite tint: it is the $s4
 *   of every block. flags is the substate word and the fade target. The
 *   constants that retail lifts to a block top (1 at entry, 2/3/4 in the
 *   attack branches) go through the multiply-assigned x and y.
 * - The row == 2 test lets cse fold row * 5 to 10 in the grid expression.
 * - commit: sits before sel = 0 so the store survives cse, and case 5 keeps
 *   its copy through the branch slot.
 * - The bank reads take the deck base through a pointer so the 0x36BC
 *   member offset stays in the load displacement.
 */
#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define D_8009B300_IN_DATA
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE91_AS_ARRAY
#define D_800EAE92_AS_ARRAY
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GDUEL_BCARDVIEWERYOFFSET_IN_DATA
#define GDUEL_BEFFECTSTATE_IN_DATA
#define GDUEL_WVIEWERCARDID_IN_DATA
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "../types.h"
#include "duel_scene_field_actions.h"
#include "duel_presentation_controls.h"
#include "duel_scene_state.h"
#include "duel_side_state.h"
#include "duel_selection_layout.h"
#include "duel_card.h"
#include "duel_card_can_act_this_turn.h"
#include "duel_card_staging.h"
#include "duel_card_display_state.h"
#include "duel_card_record_lifecycle.h"
#include "duel_cursor_status.h"
#include "duel_field_display_objects.h"
#include "duel_grid.h"
#include "duel_card_defense_rotation.h"
#include "display_object_property_transitions.h"
#include "duel_action_ready_marker.h"
#include "duel_cursor_step_toward_target.h"
#include "duel_update_card_use_confirmation.h"
#include "duel_action_lock.h"
#include "duel_card_viewer.h"
#include "duel_effect.h"
#include "duel_check_quit_input.h"
#include "duel_side_view_angles.h"
#include "ai.h"
#include "ai_controller_handoff.h"
#include "duel_load_package_stage.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object_work_slots.h"
#include "duel_get_card_viewer_request_id.h"
#include "duel_view_tween.h"
#include "input.h"
#include "sorted_entry.h"
#include "sound.h"
#include "ai_board_snapshot.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
/* The confirm buttons (Cross|Square). The Japanese release swaps Cross and
 * Circle, so a regional build names its own. */
#ifndef DUEL_FIELD_ACTIONS_CONFIRM_MASK
#define DUEL_FIELD_ACTIONS_CONFIRM_MASK 0xC0
#endif

/* The cancel button (Circle), swapped the same way. */
#ifndef DUEL_FIELD_ACTIONS_CANCEL_BUTTON
#define DUEL_FIELD_ACTIONS_CANCEL_BUTTON 0x20
#endif

/* European layout shifts both cursor field_0C values by 0x10. */
#ifndef DUEL_FIELD_ACTIONS_INITIAL_CURSOR_FIELD_0C
#define DUEL_FIELD_ACTIONS_INITIAL_CURSOR_FIELD_0C 0xAE
#endif

#ifndef DUEL_FIELD_ACTIONS_TARGET_CURSOR_FIELD_0C
#define DUEL_FIELD_ACTIONS_TARGET_CURSOR_FIELD_0C 0x74
#endif

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define SB(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define SIDE_SIZE 4
/* Record 3 of the side's selection records: the same symbol as record 0. */
#define SEL_REC3 ((DuelSelectionRecord *)(gDuel_abSelectionSides + 0x54))
#define FIELD_CURSOR (((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2])
/* Grid card, two spellings: the [side][slot] form and the flat form. */
#define GRID_CARD(s) (&gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[(s)->row * 5 + (s)->col + gDuel_bActiveSide * 20]])
#define GRID_CARD_FLAT(s) \
    (&gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[(s)->row * 5 + (s)->col + gDuel_bActiveSide * 20]])
#define BANK(i, o) (big = 0x48000, H(pw + (i) * 0x1C + big, (o)))
#define CARD_KIND(id) ((gDuel_adwCardStats[(id) - 1] >> 0x1A) & 0x1F)
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

/* Tentative definitions: the _comm profile keeps them common, which is what
 * puts retail's load-delay nop in front of each gp-relative store. */
u16 gDuel_awSavedStatModifiers;
u16 gDuel_wSavedTargetStatModifier;
u16 gDuel_awSavedRecordFlags;
u16 gDuel_wSavedTargetRecordFlags;
u8 gDuel_bCardMoveSlot;

void DuelScene_UpdateFieldActions(void)
{
    DuelSelectionRecord *side;
    DuelSelectionRecord *rec;
    u8 *o;
    u8 *other;
    DuelCardRecord *card;
    DuelCardRecord *pick;
    DisplayObject *co;
    s32 a;
    s32 k;
    s32 n;
    s32 i;
    s32 x;
    s32 y;
    s32 t;
    s32 lvl;
    s32 r;
    s32 d;
    s32 big;
    u8 *pw;
    s32 idx;
    s8 row;
    u32 f;
    s32 flags;

    side = (DuelSelectionRecord *)gDuel_abSelectionRecord2 + gDuel_bActiveSide * SIDE_SIZE;
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        if (gDuel_aSideStates[0].life_points.signed_value == 0 ||
            gDuel_aSideStates[1].life_points.signed_value == 0) {
            gDuel_bWinnerSide = gDuel_bActiveSide;
            if (gDuel_aSideStates[gDuel_bActiveSide].life_points.signed_value == 0) {
                gDuel_bWinnerSide = gDuel_bActiveSide ^ 1;
            }
            gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment = 2;
            gDuel_wSceneStateFlags = 0xC;
            return;
        }
        gDuel_pActionReadyMarker = 0;
        gDuel_bFieldActionAutoCommit = 0;
        gDuel_pActiveSelectionRecord = side;
        side->row_end = 4;
        side->field_0C = DUEL_FIELD_ACTIONS_INITIAL_CURSOR_FIELD_0C;
        y = 1;
        side->field_18 = y;
        side->field_13 = 0;
        side->row_min = 0;
        gDuel_wDisplayTransitionFlags = 3;
        gDuel_bSceneSubStep = y;
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_bSceneSubStep = 3;
        }
        return;
    case 2:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            Ai_RefreshBoardSnapshot();
            AiScript_Init(gAiScript_abFieldScript);
            gDuel_wActionSubstate = 0;
            return;
        }
        flags = (u16)gDuel_wActionSubstate;
        k = flags & 0xF;
        switch (k) {
        case 0:
            a = AiScript_Run();
            if (a == 0) {
                return;
            }
            if (a == 3) {
                gDuel_wOutcomeFlags &= 0xEFFF;
                DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
                gDuel_pActionReadyMarker = 0;
                gDuel_bSceneSubStep = 0xA;
                SD_SEPlayFull(0x30);
                return;
            }
            gDuel_wActionSubstate = 1;
            return;
        case 1:
            if (!(flags & 0x8000)) {
                a = gAi_bFieldSourceCode[0];
                gDuel_wActionSubstate = flags | 0x8000;
                gDuelCursor_Direction = -1;
                gDuelCursor_TargetRow = 2;
                gDuelCursor_TargetColumn = (a - 1) % 5;
                if (a >= 6) {
                    gDuelCursor_TargetRow = 3;
                }
            }
            if (DuelCursor_StepTowardTarget(side) != 0) {
                return;
            }
            if (gAi_Selection.bytes[0xA] == 0 && gAi_Selection.bytes[0xB] == 0) {
                card = GRID_CARD(side);
                f = card->flags;
                if (f & 0x8000) {
                    card->flags = f | 0x4000;
                    Duel_UpdateCardDisplayState(card->object);
                }
                gDuel_bSceneSubStep = 2;
                return;
            }
            gDuel_wActionSubstate = 3;
            if (gAi_Selection.bytes[0xB] != 0 && gAi_Selection.bytes[9] < 6) {
                gDuel_wActionSubstate = 2;
            }
            return;
        case 2:
            card = GRID_CARD(side);
            if (!(flags & 0x8000)) {
                f = card->flags;
                gDuel_wActionSubstate = flags | 0x8000;
                if (f & 0x8000) {
                    if (f & 0x800) {
                        card->flags = f | 0x4000;
                        Duel_UpdateCardDisplayState(card->object);
                        goto back_to_menu;
                    }
                    o = card->object;
                    DISPLAY_OBJECT_VIEW(o)->update =
                        (DisplayObjectCallback)DuelCard_UpdateDefenseRotation;
                    DISPLAY_OBJECT_VIEW(o)->field_6C = 0xF;
                    return;
                }
            back_to_menu:
                gDuel_bSceneSubStep = 2;
                return;
            }
            if (DisplayObject_FindAllocatedByTag(0xF) == 0) {
                card->flags |= 0x4000;
                Duel_UpdateCardDisplayState(card->object);
                gDuel_bSceneSubStep = k;
            }
            return;
        case 3:
            if (!(flags & 0x8000)) {
                gDuel_wActionSubstate = flags | 0x8000;
                card = GRID_CARD(side);
                if (card->flags & 0x800) {
                    gDuel_wActionSubstate = flags | 0xC000;
                    o = card->object;
                    DISPLAY_OBJECT_VIEW(o)->update =
                        (DisplayObjectCallback)DuelCard_UpdateDefenseRotation;
                    DISPLAY_OBJECT_VIEW(o)->field_6C = 0xF;
                }
                return;
            }
            if (!(flags & 0x4000) || DisplayObject_FindAllocatedByTag(0xF) == 0) {
                gDuel_bFieldActionAutoCommit = 1;
                goto attack;
            }
            return;
        }
        return;
    case 3:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0xC0;
            gDuel_bFieldActionAutoCommit = 0;
            if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0) {
                gDuel_bSceneSubStep = 2;
                return;
            }
            side->status = 0;
        }
        if (DisplayObject_FindAllocatedByTag(0xF) != 0) {
            return;
        }
        if (DuelCursor_UpdateFromInput(side) != 0) {
            gDuel_bSceneSubStep |= 0x40;
            return;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            gDuel_bSceneSubStep &= 0xBF;
            if (side->row == 2
                && DuelCard_CanActThisTurn(GRID_CARD(side)) != 0) {
                gDuel_pActionReadyMarker = Duel_GetOrCreateActionReadyMarker(side->cursor_object);
            } else {
                DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
                gDuel_pActionReadyMarker = 0;
            }
        }
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] < 0 && (gInput_wPad1Pressed & 0x800)) {
            gDuel_bSceneSubStep = 0xB;
            return;
        }
        if (Duel_CheckQuitInput() != 0) {
            return;
        }
        if (gInput_wPad1Pressed & 0xC) {
            r = side->row;
            card = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[r * 5 + side->col + gDuel_bActiveSide * 20]];
            if (r == 2 && DuelCard_CanActThisTurn(card) != 0) {
                o = card->object;
                DISPLAY_OBJECT_VIEW(o)->update =
                    (DisplayObjectCallback)DuelCard_UpdateDefenseRotation;
                DISPLAY_OBJECT_VIEW(o)->field_6C = 0xF;
                SD_SEPlayFull(0xB);
            }
            return;
        }
        a = Duel_GetCardViewerRequestId(GRID_CARD(side));
        if (a != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = a;
            gDuel_bEffectState = 2;
            return;
        }
        if (gInput_wPad1Pressed & DUEL_FIELD_ACTIONS_CONFIRM_MASK) {
            if (side->row == 2) {
                if (gDuel_pActiveSideState->swords_turns_remaining != 0) {
                    SD_SEPlayFull(9);
                    return;
                }
                if (gDuel_wOutcomeFlags & 0x1000) {
                    SD_SEPlayFull(9);
                    return;
                }
            }
            if (side->row < 2) {
                SD_SEPlayFull(9);
                return;
            }
        attack:
            i = side->row * 5 + side->col;
            card = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + gDuel_bActiveSide * 20]];
            if ((W(card, 0x14) & 0xC8000000) == 0x80000000) {
                SD_SEPlayFull(7);
                DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
                gDuel_pActionReadyMarker = 0;
                a = side->row;
                if (a >= 2) {
                    side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
                    if (a >= 3) {
                        gDuel_bSceneSubStep = 7;
                        if (CARD_KIND(card->card_id) == 0x17) {
                            x = 2;
                            y = 3;
                            side->row_min = x;
                            side->row = x;
                            side->row_end = y;
                            x = 4;
                            gDuel_bSceneSubStep = x;
                        }
                    } else {
                        x = 4;
                        y = 1;
                        gDuel_bSceneSubStep = x;
                        side->row_min = y;
                        x = 2;
                        side->row_end = x;
                        side->row = y;
                    }
                }
            } else {
                SD_SEPlayFull(9);
            }
        }
        return;
    case 4:
        side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = side;
            side->field_18 = 0;
            side->field_0C = DUEL_FIELD_ACTIONS_TARGET_CURSOR_FIELD_0C;
            Duel_CreateFieldCursorObject(side);
            Duel_StartViewTween(0x10, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][side->field_18][side->row]);
            o = (u8 *)side->cursor_object;
            other = *(u8 **)((u8 *)SEL_REC3 + gDuel_bActiveSide * 0x70 - 0x18);
            DisplayObject_ResetVelocity((void *)o);
            DISPLAY_OBJECT_VIEW(o)->field_60 = 0x10;
            DISPLAY_OBJECT_VIEW(o)->field_6C = 0;
            W(o, 0x2C) = W(o, 0x28);
            DISPLAY_OBJECT_VIEW(o)->field_34.h.field_36 =
                ((S(o, 0x28) - S(other, 0x28)) << 8) / 16;
            DISPLAY_OBJECT_VIEW(o)->field_38.h.field_3A =
                ((S(o, 0x2A) - S(other, 0x2A)) << 8) / 16;
            gDuel_wDisplayTransitionFlags = 0x52;
            DISPLAY_OBJECT_VIEW(o)->position.word =
                DISPLAY_OBJECT_VIEW(other)->position.word;
            return;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            DuelField_RefreshCursorCardInfo(side);
            gDuel_bSceneSubStep = 6;
            if (gDuel_bFieldActionAutoCommit != 0) {
                gDuel_bSceneSubStep = 5;
            }
        }
        return;
    case 5:
        side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_bAnimatedBattleRequested = 0;
            a = gAi_bFieldTargetCode[0];
            if (a < 0x38) {
                if (a >= 6) {
                    gDuelCursor_TargetColumn = a - 6;
                    x = 3;
                } else {
                    gDuelCursor_TargetColumn = a - 1;
                    x = 2;
                }
            } else {
                gDuelCursor_TargetColumn = 0x3C - a;
                x = 1;
            }
            gDuelCursor_TargetRow = x;
        }
        if (DuelCursor_StepTowardTarget(side) != 0) {
            return;
        }
        goto commit;
    case 6:
        side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
        if (gDuel_bSceneSubStep & 0x80) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = side;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (gDuel_wDisplayTransitionFlags == 0) {
                DisplayObject_ReleaseIfPresent(side->cursor_object);
                side->cursor_object = 0;
                gDuel_bSceneSubStep = 3;
                gDuel_pActiveSelectionRecord =
                    &((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2];
            }
            return;
        }
        if (DuelCursor_UpdateFromInput(side) != 0) {
            return;
        }
        if (gInput_wPad1Pressed & DUEL_FIELD_ACTIONS_CANCEL_BUTTON) {
            Duel_StartViewTween(0x10, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][FIELD_CURSOR.field_18][FIELD_CURSOR.row]);
            o = (u8 *)side->cursor_object;
            other = (u8 *)FIELD_CURSOR.cursor_object;
            DisplayObject_ResetVelocity((void *)o);
            DISPLAY_OBJECT_VIEW(o)->field_60 = 0x10;
            DISPLAY_OBJECT_VIEW(o)->field_6C = 0;
            DISPLAY_OBJECT_VIEW(o)->field_34.h.field_36 =
                ((S(other, 0x28) - S(o, 0x28)) << 8) / 16;
            DISPLAY_OBJECT_VIEW(o)->field_38.h.field_3A =
                ((S(other, 0x2A) - S(o, 0x2A)) << 8) / 16;
            W(o, 0x2C) = W(other, 0x28);
            gDuel_wDisplayTransitionFlags = 0x58;
            gDuel_bSceneSubStep |= 0x40;
            return;
        }
        a = Duel_GetCardViewerRequestId(GRID_CARD(side));
        if (a != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = a;
            gDuel_bEffectState = 2;
            return;
        }
        if (!(gInput_wPad1Pressed & DUEL_FIELD_ACTIONS_CONFIRM_MASK)) {
            return;
        }
        gDuel_bAnimatedBattleRequested = 0;
        if (gInput_wPad1Pressed & 0x80) {
            gDuel_bAnimatedBattleRequested = 1;
        }
    commit:
        a = 0;
        if (side->row == 1) {
            i = 5;
            a = 1;
            for (n = 0; n < 5; n++, i++) {
                pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + gDuel_bActiveSide * 20]];
                if (pick->flags & 0x8000) {
                    a = 0;
                    break;
                }
            }
        }
        i = side->row * 5 + side->col;
        pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + gDuel_bActiveSide * 20]];
        if (a == 0 && !(pick->flags & 0x8000)) {
            SD_SEPlayFull(9);
            return;
        }
        SD_SEPlayFull(7);
        for (n = 6; n >= 0; n--) {
            gDisplayObject_apWorkSlots[n] = 0;
        }
        i = FIELD_CURSOR.row * 5 + FIELD_CURSOR.col;
        pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + gDuel_bActiveSide * 20]];
        gDuel_awSavedRecordFlags = pick->flags;
        gDuel_awSavedStatModifiers = H(pick, 0x12);
        o = (u8 *)Duel_CreateCardDisplayObject(pick, S(pick, 8), S(pick, 0xA));
        DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), -0xA);
        gDisplayObject_apWorkSlots[0] = DISPLAY_OBJECT_VIEW(o);
        d = pick->card_id;
        DuelCard_DeactivateRecord(pick);
        i = side->row * 5 + side->col;
        pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + gDuel_bActiveSide * 20]];
        if (a == 0) {
            gDuel_wSavedTargetRecordFlags = pick->flags;
            gDuel_wSavedTargetStatModifier = H(pick, 0x12);
            o = (u8 *)Duel_CreateCardDisplayObject(pick, S(pick, 8), S(pick, 0xA));
            gDisplayObject_apWorkSlots[1] = DISPLAY_OBJECT_VIEW(o);
            gDuel_bCardMoveSlot = DISPLAY_OBJECT_VIEW(o)->field_6A;
            DuelCard_DeactivateRecord(pick);
            if (CARD_KIND(d) == 0x17) {
                gDuel_bSceneSubStep = 8;
                return;
            }
        }
        gDuel_wSceneStateFlags = 9;
        return;
    case 7:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            i = side->row * 5 + side->col;
            card = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + gDuel_bActiveSide * 20]];
            o = (u8 *)Duel_CreateCardDisplayObject(card, card->screen_x, card->screen_y);
            card->flags &= 0x7FFF;
            if (card->flags & 0x1000) {
                DISPLAY_OBJECT_VIEW(o)->field_20.b.rotation_y = 0x80;
                H(o, 8) |= 4;
            }
            DISPLAY_OBJECT_VIEW(o)->field_60 = 0xC;
            DISPLAY_OBJECT_VIEW(o)->update =
                (DisplayObjectCallback)DisplayObject_UpdatePositionRotationTransition;
            DISPLAY_OBJECT_VIEW(o)->position.h.field_28 = 0x86;
            DISPLAY_OBJECT_VIEW(o)->position.h.field_2A = 0x5A;
            DISPLAY_OBJECT_VIEW(o)->field_2C.h.field_2C = 0;
            DISPLAY_OBJECT_VIEW(o)->field_6C = 1;
            gDuel_pActionCard = DISPLAY_OBJECT_VIEW(o);
            co = side->cursor_object;
            co->flags &= 0xFFBF;
            if (gDuel_bFieldActionAutoCommit == 0) {
                gDuel_dwFieldDimTarget = 0x202020;
                card = gDuel_aCardRecords;
                for (n = 0; n < 0x1E; card++, n++) {
                    if (card->flags & 0x8000) {
                        o = card->object;
                        DISPLAY_OBJECT_VIEW(o)->field_6C = 1;
                        DISPLAY_OBJECT_VIEW(o)->field_60 = 8;
                        DISPLAY_OBJECT_VIEW(o)->update =
                            (DisplayObjectCallback)DisplayObject_UpdateColorTransition;
                        a = 0x20;
                        if (card->flags & 0x4000) {
                            a = 0x10;
                        }
                        DISPLAY_OBJECT_VIEW(o)->field_2C.h.field_2C = a;
                        DISPLAY_OBJECT_VIEW(o)->position.h.field_2A = a;
                        DISPLAY_OBJECT_VIEW(o)->position.h.field_28 = a;
                    }
                }
                gDuel_bConfirmFlags = 0;
                gDuel_bSceneSubStep |= 0x20;
            }
            return;
        }
        if (gDuel_bFieldActionAutoCommit != 0) {
            if (DisplayObject_FindAllocatedByTag(1) != 0) {
                return;
            }
            DuelCard_RemoveFromField(&gDuel_aCardRecords[B(gDuel_pActionCard, 0x6A)]);
            gDuel_bSceneSubStep = 9;
            gDisplayObject_apWorkSlots[0] = gDuel_pActionCard;
            return;
        }
        if (gDuel_bSceneSubStep & 0x20) {
            a = *(u8 *)&gModel_dwFogBaseColor;
            flags = (u8)gDuel_dwFieldDimTarget;
            if (a >= flags) {
                a -= 8;
                t = a < flags;
            } else {
                a += 8;
                t = flags < a;
            }
            if (t != 0) {
                a = flags;
            }
            t = (a << 16) | (a << 8) | a;
            gModel_dwFogBaseColor = t;
            if (t == gDuel_dwFieldDimTarget && DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep &= 0xDF;
            }
            return;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (gDuel_bSceneSubStep & 0x10) {
                gDuel_aCardRecords[B(gDuel_pActionCard, 0x6A)].flags |= 0x8000;
                DisplayObject_ReleaseIfPresent(gDuel_pActionCard);
                gDuel_bSceneSubStep = 3;
                co = side->cursor_object;
                co->flags |= 0x40;
                return;
            }
            DuelCard_RemoveFromField(&gDuel_aCardRecords[B(gDuel_pActionCard, 0x6A)]);
            gDuel_bSceneSubStep = 9;
            gDisplayObject_apWorkSlots[0] = gDuel_pActionCard;
            return;
        }
        a = Duel_UpdateCardUseConfirmation();
        if (a == 0) {
            return;
        }
        if (a != 1) {
            gDuel_bSceneSubStep |= 0x10;
            SD_SEPlayFull(8);
            o = (u8 *)gDuel_pActionCard;
            DISPLAY_OBJECT_VIEW(o)->field_60 = 0xC;
            pw = gDuel_abCardStagingBuffer;
            DISPLAY_OBJECT_VIEW(o)->update =
                (DisplayObjectCallback)DisplayObject_UpdatePositionRotationTransition;
            DISPLAY_OBJECT_VIEW(o)->position.h.field_28 =
                BANK(DISPLAY_OBJECT_VIEW(o)->field_6A, 0x36BC);
            DISPLAY_OBJECT_VIEW(o)->position.h.field_2A =
                BANK(DISPLAY_OBJECT_VIEW(o)->field_6A, 0x36BE);
            DISPLAY_OBJECT_VIEW(o)->field_2C.h.field_2C = 0;
            if (gDuel_aCardRecords[DISPLAY_OBJECT_VIEW(o)->field_6A].flags
                & 0x1000) {
                DISPLAY_OBJECT_VIEW(o)->field_2C.h.field_2C = 0x80;
            }
            DISPLAY_OBJECT_VIEW(o)->field_6C = 1;
        } else {
            SD_SEPlayFull(7);
        }
        gDuel_dwFieldDimTarget = 0x808080;
        card = gDuel_aCardRecords;
        gDuel_bSceneSubStep |= 0x60;
        for (n = 0; n < 0x1E; card++, n++) {
            if (card->flags & 0x8000) {
                o = card->object;
                DISPLAY_OBJECT_VIEW(o)->field_6C = 1;
                DISPLAY_OBJECT_VIEW(o)->field_60 = 8;
                DISPLAY_OBJECT_VIEW(o)->update =
                    (DisplayObjectCallback)DisplayObject_UpdateColorTransition;
                a = 0x80;
                if (card->flags & 0x4000) {
                    a = 0x40;
                }
                DISPLAY_OBJECT_VIEW(o)->field_2C.h.field_2C = a;
                DISPLAY_OBJECT_VIEW(o)->position.h.field_2A = a;
                DISPLAY_OBJECT_VIEW(o)->position.h.field_28 = a;
            }
        }
        return;
    case 8:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            rec = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
            co = rec->cursor_object;
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = rec;
            DisplayObject_ReleaseIfPresent(co);
            gDuel_wDisplayTransitionFlags = 8;
            gDuel_pActiveSelectionRecord->cursor_object = 0;
        }
        if (gDuel_wDisplayTransitionFlags != 0) {
            return;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            gDuel_bSceneSubStep |= 0x40;
            gDuel_wDisplayTransitionFlags = 0xC;
            gDuel_pActiveSelectionRecord =
                &((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2];
            return;
        }
        gDuel_wSceneStateFlags = 7;
        return;
    case 9:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wDisplayTransitionFlags = 0xC;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 6;
        }
        return;
    case 11:
        gDuel_wOutcomeFlags &= 0xEFFF;
        DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
        gDuel_pActionReadyMarker = 0;
        gDuel_bSceneSubStep = 0xA;
        SD_SEPlayFull(0x30);
        return;
    case 10:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wDisplayTransitionFlags = 0xC;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 0xA;
        }
        return;
    }
}
