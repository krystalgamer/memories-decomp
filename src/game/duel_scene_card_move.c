#define DUEL_CARD_STAGING_DECK_VIEW
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE8F_IS_ARRAY
#define GDIALOG_CHOICE_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#define FUNC_80018004_AMBIENT_POSITION_ARGS
#include "../types.h"
#include "duel_scene_state.h"
#include "ai.h"
#include "display_object_work_slots.h"
#include "display_object_interpolation.h"
#include "display_object_core.h"
#include "duel_card_display_objects.h"
#include "duel_card.h"
#include "duel_card_staging.h"
#include "duel_card_record_lifecycle.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "duel_scene_callbacks.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_trap_resolution.h"
#include "duel_screen_tables.h"
#include "dialog_choice.h"
#include "dialog_read_choice_input.h"
#include "text_box_lifecycle.h"
#include "text_box_update_channels.h"
#include "duel_select_trap_by_card_id.h"
#include "duel_effect_set_entry_range_layout.h"
#include "display_object_config.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_display_state.h"
#include "duel_selection_layout.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "duel_hand_position_selection_cursor.h"
#include "duel_hand_selection_cursor.h"
#include "duel_display.h"
#include "duel_hand.h"
#include "duel_hand_selection_colors.h"
#include "duel_deck_card.h"
#include "ai_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "../unmatched.h"
#include "duel_field_selection.h"

/* One screen height above the top: -0xF0 in the US build, -0x100 in the
   European one, whose wrapper defines its own. */
#ifndef DUEL_CARD_OFFSCREEN_Y
#define DUEL_CARD_OFFSCREEN_Y -0xF0
#endif
#include "input.h"
#include "sound.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"

#ifndef DUEL_CARD_STAGING_CONFIRM_MASK
#define DUEL_CARD_STAGING_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

#define DISPLAY_OBJECT_POSITION_VIEW(object) \
    ((DisplayObjectPosition *)(object))
#define DISPLAY_OBJECT_SNAPSHOT_VIEW(object) \
    ((DisplayObjectSnapshot *)(object))

void DuelScene_UpdateCardMove(void)
{
    DisplayObject *object;
    DuelEffectChannel *box;
    DuelCardRecord *card;
    DuelCardRecord *scratch;
    DuelEffectRequest *request;
    u16 flags;
    u16 z;
    s32 slot;

    object = gDisplayObject_apWorkSlots[0];
    if (!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        if (gDuel_wSceneStateFlags & 0x4000) {
            goto state_four;
        }
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0 &&
            object->field_68 < 0x14) {
            gDuel_aCardRecords[object->field_6A].flags &= ~0x200;
            if (gAi_bGuardianStarChoice[0] & 1) {
                gDuel_aCardRecords[object->field_6A].flags |= 0x200;
            }
            goto state_four;
        }
        gDuel_bSceneSubStep = 1;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
        object->field_60 = 0;
        if ((s16)object->field_30.h.field_30 == 0x86 ||
            (s16)object->field_30.h.field_32 == 0x2A) {
            gDuel_bSceneSubStep = 2;
            return;
        }
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        DisplayObject_InterpolatePositionCosine(
            DISPLAY_OBJECT_POSITION_VIEW(object),
            0x86, 0x2A, object->field_60);
        object->field_60 += 0x80;
        if (object->field_60 < 0x800) {
            break;
        }
        object->field_30.h.field_30 = 0x86;
        object->field_30.h.field_32 = 0x2A;
        gDuel_bSceneSubStep = 2;
        /* Fall through to the next presentation phase in the same frame. */
    case 2:
        if (object->field_68 >= 0x14) {
            goto state_four;
        }
        gDuel_bSceneSubStep = 3;
        /* Fall through to initialize the choice dialog. */
    case 3:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wSelectedCardID =
                D_8015C424_cards.field_cards[object->field_6A].card_id;
#ifdef VERSION_JAPAN
            box = TextBox_CreateFlagged(0, 0x21, 0x50, 0x6E, 0xA0, 0x30, 0x20);
#elif defined(VERSION_EUROPE)
            /* The European build makes neither the func_8003B6AC call nor
               the two field stores, and flags the box 0x40. */
            box = TextBox_CreateFlagged(0, 0x21, 0x48, 0x6E, 0xB0, 0x30, 0x40);
#else
            func_8003B6AC(0, 0xB);
            box = TextBox_CreateFlagged(0, 0x21, 0x48, 0x6E, 0xB0, 0x30, 0x20);
            box->glyph_advance_x = 8;
            box->line_advance_y = 0x10;
#endif
            do {
                TextBox_UpdateChannels();
            } while (box->prompt_object == 0);
            break;
        }
        if (gDuel_bSceneSubStep & 0x10) {
            gDuel_aCardRecords[object->field_6A].flags &= ~0x200;
            if (gDialog_bChoice) {
                gDuel_aCardRecords[object->field_6A].flags |= 0x200;
            }
            TextBox_Destroy(gText_aChannels);
state_four:
            gDuel_bSceneSubStep = 4;
            break;
        }
        if (!Dialog_ReadChoiceInput(gText_aChannels) &&
            (gInput_wPad1Pressed & DUEL_CARD_STAGING_CONFIRM_MASK)) {
            SD_SEPlayFull(7);
            gDuel_bSceneSubStep |= 0x10;
        }
        break;
    case 4:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            object->field_2C.h.field_2E = Duel_ProjectCardSlotScreenX(gDuel_bCardMoveSlot) - 0x1E;
            DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
            object->field_60 = 0x400;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(object),
                             object->field_2C.h.field_2E, -0xBC,
                             object->field_60);
            object->field_60 -= 0x2A;
            if (object->field_60 > 0) {
                break;
            }
            gDuel_bSceneSubStep |= 0x40;
            scratch = (DuelCardRecord *)0x1F800000;
            *scratch = gDuel_aCardRecords[object->field_6A];
            Duel_SetupFieldCardFromDeck(gDuel_bCardMoveSlot, object->field_6B);
            card = &gDuel_aCardRecords[gDuel_bCardMoveSlot];
            flags = card->flags | (scratch->flags & 0x7E00);
            card->flags = flags & ~0x400;
            if (!(flags & 0x1000)) {
                card->flags = flags & ~0x2400;
            }
            card->stat_modifier = scratch->stat_modifier;
            DisplayObject_ReleaseIfPresent(object);
            gDisplayObject_apWorkSlots[0] = card->object;
            object = gDisplayObject_apWorkSlots[0];
            Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)object);
            DISPLAY_OBJECT_POSITION_VIEW(object)->out_y = DUEL_CARD_OFFSCREEN_Y;
            DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
            object->field_60 = -0x400;
            break;
        }
        Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(object),
                         (s16)object->field_30.h.field_30, -0x18,
                         object->field_60);
        object->field_60 += 0x2A;
        if (object->field_60 < 0) {
            break;
        }
        DISPLAY_OBJECT_POSITION_VIEW(object)->out_y = -0x18;
        gDuel_bSceneSubStep = 5;
        SD_SEPlayFull(0xC);
        break;
    case 5:
        if (gDuel_wPendingEquipBonus &&
            Duel_SelectTrapByCardId(DUEL_REVERSE_TRAP_CARD_ID)) {
            gDuel_bSceneSubStep = 6;
            break;
        }
        gDuel_wSceneStateFlags = 5;
        break;
    case 6:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0xC0;
            gDuel_wTrapPresentationMode = 0;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (Duel_UpdateTrapPresentation()) {
                break;
            }
            request = DuelEffect_CreateRequest(0xD);
            request->field_00 = object->field_30.h.field_30;
            request->field_02 = object->field_30.h.field_32;
            z = object->field_34.h.field_34;
            request->field_12 = -gDuel_wPendingEquipBonus;
            request->field_04 = z;
            SD_SEPlayFull(0x21);
            gDuel_bSceneSubStep &= ~0x40;
            break;
        }
        slot = object->field_6A;
        gDuel_wSceneStateFlags = 5;
        card = &gDuel_aCardRecords[slot];
        card->stat_modifier -= gDuel_wPendingEquipBonus * 2;
        break;
    }
}

/* Regional values: the inner object's y (194 -> 210), and the count sprite's
 * field_5D (0xB8 -> 0xB0) and field_40, its CLUT x (0x250 -> 0x2D0). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_HAND_STACK_INNER_Y
#define DUEL_HAND_STACK_INNER_Y 194
#endif
#ifndef DUEL_HAND_STACK_COUNT_FIELD_5D
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB8
#endif
#ifndef DUEL_HAND_STACK_COUNT_CX
#define DUEL_HAND_STACK_COUNT_CX 0x250
#endif

#define DUEL_CARD_DISPLAY_OBJECT_VIEW(object) \
    ((DuelCardDisplayObject *)(object))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

void DuelHand_PositionSelectionCursor(DuelSelectionRecord *object)
{
    DisplayObject *inner = object->cursor_object;

    inner->field_30.h.field_30 = (s8)object->field_0E * 60 + 14;
    inner->field_30.h.field_32 = DUEL_HAND_STACK_INNER_Y;
}

void DuelHand_CreateSelectionOrderMarker(DuelSelectionRecord *arg)
{
 register DuelSelectionRecord*object=arg;register DuelHandSlot*slot=&gDuelHand_aSlots[(s8)object->field_0E];register DuelCardDisplayObject*child;
 DUEL_CARD_DISPLAY_OBJECT_VIEW(slot->object)->out_y-=4;child=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),1);DisplayObject_ConfigureScreenSprite(DISPLAY_OBJECT_VIEW(child),DUEL_CARD_DISPLAY_OBJECT_VIEW(slot->object)->out_x,DUEL_CARD_DISPLAY_OBJECT_VIEW(slot->object)->out_y,0x10,0x10,object->field_15<<4,DUEL_HAND_STACK_COUNT_FIELD_5D,0xB,DUEL_HAND_STACK_COUNT_CX,0xFC);
 DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(child));DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(child),(s8)((u8)slot->object->depth_offset+1));slot->order_marker=DISPLAY_OBJECT_VIEW(child);object->field_15++;slot->selection_order=object->field_15;SD_SEPlayFull(0x2F);
}

void DuelHand_RefreshSelectionColors(DuelSelectionRecord *side) {
    s32 i;
    for (i = 0; i < HAND_SIZE; i++)
        if (gDuelHand_aSlots[i].selection_order == 0)
            gDuelHand_aSlots[i].object->rgb =
                DUEL_DISPLAY_COLOR_DIMMED;
    if (side->field_15 == 0)
        gDuelHand_aSlots[(s8)side->field_0E].object->rgb =
            DUEL_DISPLAY_COLOR_NORMAL;
}

/* Regional value: the cursor's field_0C (0x74 -> 0x84). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_SELECTION_CURSOR_FIELD_0C
#define DUEL_SELECTION_CURSOR_FIELD_0C 0x74
#endif

/* Private helpers of the same duel action controller,
   DuelScene_UpdateHandActions:
   field-placement selection setup followed by execution of the
   AI-script hand/field selection in state 0. */

void Duel_BeginFieldPlacementSelection(DuelSelectionRecord *selection) {
    DisplayObject *r;
    DuelCardRecord *e;
    u8 *b;
    DuelCardReplayRecordBlock *g;
    s32 k;
    s32 c;
    s32 i;

    gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionRecord2 +
                                       gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
    gDuel_pActiveSelectionRecord->status = 0;
    gDuel_pActiveSelectionRecord->field_13 = 1;
    gDuel_pActiveSelectionRecord->field_0C = DUEL_SELECTION_CURSOR_FIELD_0C;
    gDuel_pActiveSelectionRecord->field_18 = 0;
    gDuel_pActiveSelectionRecord->row_min = 2;
    gDuel_pActiveSelectionRecord->row_end = 3;

    if (selection->field_15 == 0) {
        b = gDuel_abCardStagingBuffer;
        r = gDuelHand_aSlots[(s8)selection->field_0E].object;
        g = (DuelCardReplayRecordBlock *)(
            (u8 *)&((DuelCardRecord *)b)[r->field_6A] +
            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
        i = (gDuel_adwCardStats[((DuelDeckCardRecord *)g->record.data)->id - 1] >>
             CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    k = i;
        if (k >= CARD_TYPE_MAGIC) {
            if (r->field_20.b.rotation_y != 0 || k == CARD_TYPE_TRAP) {
                gDuel_pActiveSelectionRecord->row_min = 3;
                gDuel_pActiveSelectionRecord->row_end = 4;
            }
        }
    }

    gDuel_pActiveSelectionRecord->row = gDuel_pActiveSelectionRecord->row_min;
    c = gDuel_abFieldGridCardRecordIndices[
        gDuel_pActiveSelectionRecord->row * DUEL_FIELD_ROW_SIZE +
        gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ];
    e = &gDuel_aCardRecords[c];

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++, e++) {
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) == 0) {
            gDuel_pActiveSelectionRecord->col = i;
            break;
        }
    }

    gDuel_wDisplayTransitionFlags = 3;
}

/* The deck records sit 0x31E0 bytes below the active-card table, and retail
   forms their base from that table's address rather than from a label of its
   own. */
#define DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS \
    ((DuelDeckCardRecord *)((u8 *)gDuel_aActiveCards - 0x31E0))

/* Applies gAi_Selection to the hand: reserves existing hand picks in the
 * local availability list, swaps pool-card records into other occupied hand
 * slots, rebuilds their objects, and rewrites picks as hand-slot codes.
 *
 * The slot search is a goto loop inside a one-pass do/while, and the order
 * and card-record bases are added as integers: those forms keep the retail
 * loop-invariant placement, operand order and register roles. */
void Duel_ApplyAiHandSelection(void)
{
    s8 sel[HAND_SIZE];
    DuelDeckCardRecord tmp;
    DuelDeckCardRecord *rec;
    DuelDeckCardRecord *other;
    DuelCardDisplayObject *spawned;
    DuelHandSlot *slot;
    DuelCardRecord *records;
    AiActiveCard *card;
    u8 *order;
    s32 i;
    s32 j;
    s32 v;
    s32 id;
    s32 k;

    for (i = 0; i < HAND_SIZE; i++) {
        sel[i] = gDuel_pActiveSideState->hand[i];
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = gAi_Selection.bytes[i];
        if (v == 0) {
            break;
        }
        if (v < 0x10) {
            sel[v - 0xB] = -1;
        }
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = gAi_Selection.bytes[i];
        if (v == 0) {
            return;
        }
        if (v >= 0x10) {
            do {
                j = 0;
                order = gDuel_abHandCardRecordIndices;
                records = gDuel_aCardRecords;
                slot = gDuelHand_aSlots;
            search:
                if (sel[j] >= 0) {
                    card = &gDuel_aActiveCards[v];
                    rec = DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS + sel[j];
                    other = DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS +
                            card->deck_index;
                    v = rec->deck_index;
                    rec->deck_index = other->deck_index;
                    other->deck_index = v;
                    tmp = *rec;
                    *rec = *other;
                    *other = tmp;
                    k = gDuel_bActiveSide * HAND_SIZE;
                    id = *(u8 *)((j + k) + (s32)order);
                    spawned = (DuelCardDisplayObject *)slot->object;
                    Duel_SetupCardRecord(id, rec->deck_index);
                    slot->object = (DisplayObject *)Duel_CreateHandCardDisplayObject(
                        (DuelCardRecord *)(id * DUEL_CARD_RECORD_SIZE +
                                           (s32)records),
                        spawned->out_x, spawned->out_y);
                    DisplayObject_ReleaseIfPresent(spawned);
                    gDuel_pActiveSideState->hand[j] = rec->deck_index;
                    gAi_Selection.bytes[i] = j + 0xB;
                    sel[j] = -1;
                    continue;
                }
                j++;
                slot++;
                if (j < HAND_SIZE) {
                    goto search;
                }
            } while (0);
        }
    }
}

#undef DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS
