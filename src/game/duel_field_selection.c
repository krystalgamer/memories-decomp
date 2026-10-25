#define FUNC_80018004_AMBIENT_POSITION_ARGS
#include "../types.h"
#include "duel_deck_card.h"
#include "duel_card_staging.h"
#include "duel_side_state.h"
#include "ai.h"
#include "duel_card.h"
#include "duel_card_display_state.h"
#include "duel_card_display_objects.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "display_object.h"
#include "display_object_core.h"
#include "../unmatched.h"
#include "duel_field_selection.h"

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
            if (r->field_20.b.field_21 != 0 || k == CARD_TYPE_TRAP) {
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
