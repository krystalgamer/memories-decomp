/*
 * Duel scene-state 4: the player's hand. It builds the hand cursor, runs the
 * scripted selection when the side is computer controlled, lets the player
 * walk the hand, inspect, flip and mark cards, and hands the marked cards to
 * the placement state in the order they were picked.
 *
 * Matches retail under gcc_2_8_1_g8_split. What the listing fixes about the
 * source:
 * - The pad words are volatile: every test re-reads them.
 * - The AI selection bytes are read as elements of D_800EAE88 ([1], [6] and
 *   [8]). spimdisasm names those addresses D_800EAE89, D_800EAE8E and
 *   D_800EAE90, but the symbol-plus-offset form is what puts the %hi ahead of
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
 *   func_8001BAF0 next door has the same one-pass do/while.
 */
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define GDUEL_BEFFECTSTATE_IN_DATA
#define GDUEL_BCARDVIEWERYOFFSET_IN_DATA
#define GDUEL_WVIEWERCARDID_IN_DATA
#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE88_VISIBLE
#define D_800EAE88_AS_BYTES
#define FUNC_80024088_WIDE_DIRECTION
#include "../types.h"
#include "../psyq/stdio.h"
#include "duel_scene_hand_actions.h"
#include "duel_scene_state.h"
#include "duel_scene_resume.h"
#include "duel_side_state.h"
#include "duel_selection_layout.h"
#include "func_8001B938.h"
#include "func_8001B8B8.h"
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
#include "func_8001B780.h"
#include "func_8001B7AC.h"
#include "duel_get_card_viewer_request_id.h"
#include "duel_check_quit_input.h"
#include "input.h"
#include "ai.h"
#include "script_state.h"
#include "sound.h"
#include "duel_card_state_helpers.h"
#include "../unmatched.h"

#define SUBSTATE (*(u16 *)D_8009B20C)
#define SIDE_CURSOR(base) \
    ((DuelCardPickCursor *)((base) + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE))
#define HAND_CARD_INDEX(side) \
    (D_800907CC[(s8)(side)->field_0E + D_8009B1D5 * 5])
#define FIELD_CURSOR \
    (((DuelSelectionSideCursors *)D_800E9F10)[D_8009B1D5].field)
#define CARD_RECORD(i) (((DuelCardStagingDeckView *)D_8015C424)->field_cards[i])
#define CARD_ID(i) (CARD_RECORD(i).card_id)
#define CARD_KIND(id) \
    ((gDuel_adwCardStats[(id) - 1] >> CARD_STAT_TYPE_SHIFT) & \
     CARD_STAT_TYPE_MASK)
#define HAND_CARD_OBJECT_VIEW(object) ((HandCardObject *)(object))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
/* The two choice sprites set their flags through the halfword, not the
 * record: the store stays ahead of the pointer's own store. */
#define SPRITE_FLAGS(sprite) (*(u16 *)((u8 *)(sprite) + 8))
#define CARD_PLACE_X(card) (*(s16 *)((u8 *)(card) + 8))
#define CARD_PLACE_Y(card) (*(s16 *)((u8 *)(card) + 0xA))

void DuelScene_UpdateHandActions(void)
{
    DuelSelectionRecord *side;
    DuelHandSlot *hand;
    DuelHandSlot *slot;
    DuelHandSlot *first_slot;
    DisplayObject **out;
    u8 *sprite;
    HandCardObject *obj;
    DuelCardRecord *card;
    s32 n;
    s32 i;
    s32 v;
    s32 value;
    s32 card_id;

    side = (DuelSelectionRecord *)(D_800E9F10 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE);
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        for (n = 6; n >= 0; n--) {
            D_800E9EF0[n] = 0;
        }
        D_8009B18C = 0;
        D_8009B188 = 0;
        D_8009B1B4 = (DuelCardPickCursor *)side;
        D_8009B162 = 0;
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
        func_8001B780((DuelHandStackState *)side);
        func_80023144((DuelFieldDisplaySource *)side, HAND_CARD_INDEX(side));
        D_8009B174 = 1;
        if (D_8009B360[D_8009B1D5] >= 0) {
            D_8009B174 = 2;
        }
        return;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        Duel_UpdateCardPickCursor(D_8009B1B4);
        if (D_8009B1D4 != 0) {
            return;
        }
        D_8009B1B4 = SIDE_CURSOR(D_800E9F10);
        gDuel_wSceneStateFlags &= 0xBFFF;
        return;
    }

    hand = &D_800EA030[(s8)side->field_0E];
    switch (D_8009B174 & 0xF) {
    case 2:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            SUBSTATE = 0;
        }
        switch (SUBSTATE & 0xF) {
        case 0:
            if (!(SUBSTATE & 0x8000)) {
                SUBSTATE |= 0x8000;
                func_80028220();
                AiScript_Init(D_801A8000);
                return;
            }
            if (AiScript_Run() != 0) {
                *(s8 *)&D_8009B1E2 = -1;
                D_8009B19E = 0;
                if (D_800EAE88[1] != 0) {
                    D_8009B19E = 1;
                }
                SUBSTATE = 1;
                func_8001BAF0();
                return;
            }
            return;
        case 1:
            if (!(SUBSTATE & 0x8000)) {
                SUBSTATE |= 0x8000;
                D_8009B1E2++;
                value = D_800EAE88[(s8)D_8009B1E2];
                if (value == 0) {
                    func_8001B8B8(side);
                    SUBSTATE = 2;
                    D_8009B19E &= 1;
                    if (D_8009B19E != 0) {
                        SUBSTATE = 3;
                    }
                    SD_SEPlayFull(7);
                    return;
                }
                if (value < 0xB) {
                    printf(D_80010060);
                    return;
                }
                D_8009B208[2] = value - 0xB;
                D_8009B1E4 = 6;
            }
            D_8009B1E4--;
            if ((s16)D_8009B1E4 <= 0) {
                D_8009B1E4 = 6;
                if ((s8)side->field_0E != D_8009B208[2]) {
                    SD_SEPlayFull(6);
                    if ((s8)side->field_0E > D_8009B208[2]) {
                        side->field_0E -= 1;
                    } else {
                        side->field_0E += 1;
                    }
                    func_8001B780((DuelHandStackState *)side);
                    func_80023144((DuelFieldDisplaySource *)side, HAND_CARD_INDEX(side));
                    return;
                }
                if (D_8009B19E & 1) {
                    func_8001B7AC((DuelHandStackState *)side);
                }
                SUBSTATE &= 0x7FFF;
                return;
            }
            return;
        case 2:
            obj = HAND_CARD_OBJECT_VIEW(hand->object);
            if (!(SUBSTATE & 0x8000)) {
                SUBSTATE |= 0xC000;
                card_id = CARD_ID(obj->card_index);
                if (CARD_KIND(card_id) < CARD_TYPE_MAGIC) {
                    if (D_800EAE88[8] == 0) {
                        SUBSTATE = 3;
                        return;
                    }
                }
                obj->target.xy.x = 0x86;
                obj->target.xy.y = 0x5A;
                obj->saved.xy.x = 0x10;
                obj->field_6C = 1;
                obj->update = func_8001EC70;
                DisplayObject_SetDepthOffset(
                    DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth + 4));
                side->cursor_object->flags &= 0xFFBF;
                return;
            }
            if (SUBSTATE & 0x4000) {
                if (DisplayObject_FindAllocatedByTag(1) == 0) {
                    SUBSTATE &= 0xBFFF;
                    if (SUBSTATE & 0x1000) {
                        DisplayObject_SetDepthOffset(
                            DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth - 4));
                        SUBSTATE = 3;
                        side->cursor_object->flags |= 0x40;
                        return;
                    }
                    obj->saved.xy.x = obj->home_x;
                    obj->saved.xy.y = obj->home_y;
                    if (D_800EAE88[8] != 0) {
                        SD_SEPlayFull(0xB);
                        SUBSTATE |= 0x2000;
                        obj->flags |= 4;
                        return;
                    }
                }
            } else if (SUBSTATE & 0x2000) {
                obj->face += 8;
                if ((s8)obj->face < 0) {
                    obj->face = 0x80;
                    D_801A7AD8[obj->card_index].flags |= 0x1000;
                    SUBSTATE &= 0xDFFF;
                    return;
                }
            } else {
                card_id = CARD_ID(obj->card_index);
                value = CARD_KIND(card_id);
                if (value >= CARD_TYPE_MAGIC && obj->face == 0 &&
                    value != CARD_TYPE_EQUIP && value != CARD_TYPE_TRAP) {
                    D_800E9EF0[0] = DISPLAY_OBJECT_VIEW(obj);
                    D_8009B1C8->hand[(s8)side->field_0E] = -1;
                    D_8009B174 = 5;
                    return;
                }
                obj->update = func_8001EC70;
                SUBSTATE |= 0x5000;
                *(s32 *)&obj->target = *(s32 *)&obj->saved;
                obj->saved.xy.x = 0x10;
                obj->field_6C = 1;
                return;
            }
            return;
        case 3:
            if (D_8009B162 != 0) {
                D_8009B160 = -1;
                return;
            }
            if (!(SUBSTATE & 0x8000)) {
                SUBSTATE |= 0xC000;
                func_8001B938(side);
                D_8009B208[2] = D_800EAE88[6] - 6;
                if (D_8009B208[2] < 0) {
                    D_8009B208[2] = D_800EAE88[6] - 1;
                }
                D_8009B160 = -1;
                return;
            }
            if (func_80024088((void *)D_8009B1B4, D_8009B160) == 0) {
                if (D_8009B1B4->col == D_8009B208[2]) {
                    goto confirm;
                }
                D_8009B160 = 0;
                if (D_8009B208[2] < D_8009B1B4->col) {
                    D_8009B160 = 2;
                }
            }
            return;
        }
        break;
    case 1:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            for (n = 0; n < 5; n++) {
                HAND_CARD_OBJECT_VIEW(D_800EA030[n].object)->color = 0x808080;
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
            func_8001B780((DuelHandStackState *)side);
            func_80023144((DuelFieldDisplaySource *)side, HAND_CARD_INDEX(side));
            SD_SEPlayFull(6);
            return;
        }
        if ((gInput_wPad1Pressed & 0xFFFF) == 0x1000 && gInput_wPad1Held == 0x1000) {
            if (hand->active_09 == 0) {
                func_8001B7AC((DuelHandStackState *)side);
                return;
            }
        } else if ((gInput_wPad1Pressed & 0x20) ||
                   ((gInput_wPad1Pressed & 0xFFFF) == 0x4000 &&
                    gInput_wPad1Held == 0x4000)) {
            value = hand->active_09;
            if (value != 0) {
                SD_SEPlayFull(0x2F);
                hand->active_09 = 0;
                HAND_CARD_OBJECT_VIEW(hand->object)->pos.xy.y += 4;
                DisplayObject_ReleaseIfPresent(hand->child);
                hand->child = 0;
                side->field_15--;
                hand = D_800EA030;
                for (n = 0; n < 5; n++, hand++) {
                    v = hand->active_09;
                    if (v >= value) {
                        hand->active_09 = v - 1;
                        HAND_CARD_OBJECT_VIEW(hand->child)->icon_state =
                            (v - 2) * 0x10;
                    }
                }
                return;
            }
        } else {
            if (gInput_wPad1Held & 3) {
                D_8009B1D4 = 0;
                D_8009B1B4 = SIDE_CURSOR(D_800E9F2C);
                gDuel_wSceneStateFlags |= 0x4000;
                return;
            }
            if ((gInput_wPad1Pressed & 0xC0) && side->field_15 != 1) {
                SD_SEPlayFull(7);
                D_8009B174 = 4;
                func_8001B8B8(side);
                if (side->field_15 == 0) {
                    D_8009B174 = 3;
                    return;
                }
            }
        }
        break;
    case 3:
        obj = HAND_CARD_OBJECT_VIEW(hand->object);
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0xC0;
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
        if (D_8009B174 & 0x40) {
            DisplayObject_InterpolatePositionCosine(
                (void *)obj, obj->target.xy.x, obj->target.xy.y, obj->step);
            obj->step = obj->step + 0xAA;
            if (obj->step >= 0x800) {
                D_8009B174 &= 0xBF;
                obj->pos.word = obj->target.word;
                if (D_8009B174 & 0x10) {
                    side->cursor_object->flags |= 0x40;
                    DisplayObject_SetDepthOffset(
                        DISPLAY_OBJECT_VIEW(obj), (s8)(obj->depth - 4));
                    if (D_8009B174 & 0x20) {
                        D_8009B174 = 4;
                        return;
                    }
                    D_8009B174 = 1;
                    return;
                }
                card_id = ((DuelDeckCardRecord *)CARD_RECORD(obj->card_index).data)->id;
                value = CARD_KIND(card_id);
                if ((value < CARD_TYPE_MAGIC || value == CARD_TYPE_TRAP) && obj->face == 0) {
                    obj->step = 0x10;
                    D_8009B174 |= 0x20;
                    SD_SEPlayFull(0xB);
                }
                sprite = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(
                    sprite, (s16)obj->pos.xy.x - 8, (s16)obj->pos.xy.y + 0x1E,
                    3, 1, 2, 0xB, 0x20C);
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(sprite));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(sprite), 0xA);
                SPRITE_FLAGS(sprite) |= 0x28;
                D_8009B188 = DISPLAY_OBJECT_VIEW(sprite);
                sprite = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(
                    sprite, (s16)obj->pos.xy.x + 0x3C, (s16)obj->pos.xy.y + 0x1E,
                    3, 1, 0, 0xB, 0x20C);
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(sprite));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(sprite), 0xA);
                SPRITE_FLAGS(sprite) |= 0x28;
                D_8009B18C = DISPLAY_OBJECT_VIEW(sprite);
                return;
            }
        } else if (D_8009B174 & 0x20) {
            obj->flags |= 4;
            obj->face += obj->step;
            if (!(obj->face & 0x7F)) {
                if (obj->face == 0) {
                    obj->flags &= 0xFFFB;
                }
                D_8009B174 &= 0xDF;
                if (D_8009B174 & 0x10) {
                restore:
                    DisplayObject_ReleaseIfPresent(D_8009B188);
                    DisplayObject_ReleaseIfPresent(D_8009B18C);
                    D_8009B18C = 0;
                    D_8009B188 = 0;
                    *(s32 *)&obj->target = *(s32 *)&obj->saved;
                    DisplayObject_SavePosition((void *)obj);
                    obj->step = 0;
                    D_8009B174 |= 0x50;
                    return;
                }
            }
        } else {
            if (gInput_wPad1Repeat & 0xA000) {
                obj->step = 0x10;
                D_8009B174 |= 0x20;
                if (gInput_wPad1Repeat & 0x2000) {
                    obj->step = -0x10;
                }
                SD_SEPlayFull(0xB);
                return;
            }
            if (gInput_wPad1Pressed & 0x20) {
                SD_SEPlayFull(8);
                if (obj->face != 0) {
                    obj->step = 0x10;
                    D_8009B174 |= 0x30;
                    return;
                }
                goto restore;
            }
            if (gInput_wPad1Pressed & 0xC0) {
                SD_SEPlayFull(7);
                D_801A7AD8[obj->card_index].flags &= 0xEFFF;
                if (obj->face != 0) {
                    D_801A7AD8[obj->card_index].flags |= 0x1000;
                }
                card_id = CARD_ID(obj->card_index);
                value = CARD_KIND(card_id);
                if (value >= CARD_TYPE_MAGIC && obj->face == 0 &&
                    value != CARD_TYPE_EQUIP && value != CARD_TYPE_TRAP) {
                    DisplayObject_ReleaseIfPresent(D_8009B188);
                    DisplayObject_ReleaseIfPresent(D_8009B18C);
                    D_800E9EF0[0] = DISPLAY_OBJECT_VIEW(obj);
                    D_8009B18C = 0;
                    D_8009B188 = 0;
                    D_8009B1C8->hand[(s8)side->field_0E] = -1;
                    D_8009B174 = 5;
                    return;
                }
                D_8009B174 |= 0x70;
                goto restore;
            }
        }
        break;
    case 4:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B210 = 0;
            func_8001B938(side);
            D_8009B174 |= 0x40;
        }
        if (D_8009B174 & 0x40) {
            if (D_8009B162 == 0) {
                D_8009B174 &= 0xBF;
                if (D_8009B174 & 0x10) {
                    D_8009B174 = 1;
                    D_8009B1B4 = SIDE_CURSOR(D_800E9F10);
                    if (side->field_15 == 0) {
                        D_8009B174 = 3;
                        return;
                    }
                }
            }
        } else if (func_80024060((void *)D_8009B1B4) == 0) {
            value = Duel_GetCardViewerRequestId(
                &D_801A7AD8[D_800907D8[FIELD_CURSOR.row * 5 + FIELD_CURSOR.col +
                                       D_8009B1D5 * 20]]);
            if (value != 0) {
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_wViewerCardID = value;
                gDuel_bEffectState = 2;
                return;
            }
            if (gInput_wPad1Pressed & 0x20) {
                D_8009B162 = 0xC;
                D_8009B174 |= 0x50;
                SD_SEPlayFull(8);
                return;
            }
            if (gInput_wPad1Pressed & 0xC0) {
            confirm:
                SD_SEPlayFull(7);
                for (n = 6; n >= 0; n--) {
                    D_800E9EF0[n] = 0;
                }
                n = D_8009B1B4->row * 5 + D_8009B1B4->col;
                value = D_800907D8[n + D_8009B1D5 * 20];
                card = &D_801A7AD8[value];
                D_8009B19C = value;
                i = 0;
                if (card->flags & 0x8000) {
                    obj = HAND_CARD_OBJECT_VIEW(card->object);
                    i = 1;
                    D_800E9EF0[0] = DISPLAY_OBJECT_VIEW(
                        func_80017F04(
                            card, CARD_PLACE_X(card), CARD_PLACE_Y(card)));
                    DuelCard_DeactivateRecord(&D_801A7AD8[obj->card_index]);
                } else if (side->field_15 == 0 &&
                           HAND_CARD_OBJECT_VIEW(hand->object)->kind ==
                               CARD_TYPE_EQUIP &&
                           D_8009B1B4->row == 2) {
                    return;
                }
                if (side->field_15 == 0) {
                    hand->active_09 = 1;
                    if (HAND_CARD_OBJECT_VIEW(hand->object)->face != 0) {
                        D_8009B1C8->rank.face_down_plays++;
                    }
                }
                /* Collect the marked cards in pick order. A missing order
                 * ends the walk: 8 is past the loop bound. */
                for (value = 1, first_slot = D_800EA030; value < 6; value++) {
                    s8 none = -1;

                    do {
                        n = 0;
                        out = &D_800E9EF0[i];
                        slot = first_slot;
                        for (;;) {
                            if (slot->active_09 == value) {
                                D_8009B1C8->hand[n] = none;
                                i++;
                                *out = DISPLAY_OBJECT_VIEW(slot->object);
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
                D_8009B174 = 6;
                return;
            }
        }
        break;
    case 6:
        if (!(D_8009B174 & 0x80)) {
            D_8009B162 = 0xC;
            D_8009B174 |= 0x80;
            D_8009B1B4 = SIDE_CURSOR(D_800E9F48);
        }
        if (D_8009B162 == 0) {
            gDuel_wSceneStateFlags = 7;
        }
        break;
    case 5:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            hand->object = 0;
            D_8009B162 = 8;
            D_8009B1B4 = SIDE_CURSOR(D_800E9F10);
        }
        if (D_8009B162 == 0) {
            gDuel_wSceneStateFlags = 6;
        }
        break;
    }
}

#undef SUBSTATE
#undef SIDE_CURSOR
#undef HAND_CARD_INDEX
#undef FIELD_CURSOR
#undef CARD_RECORD
#undef CARD_ID
#undef CARD_KIND
#undef SPRITE_FLAGS
#undef CARD_PLACE_X
#undef CARD_PLACE_Y
