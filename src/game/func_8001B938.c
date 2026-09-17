#define FUNC_80018004_AMBIENT_POSITION_ARGS
#define D_800EAE88_VISIBLE
#define D_800EAE88_AS_BYTES
#include "../types.h"
#include "duel_deck_card.h"
#include "duel_card_staging.h"
#include "duel_side_state.h"
#include "ai.h"
#include "duel_card.h"
#include "duel_card_display_state.h"
#include "func_80018004.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "display_object.h"
#include "display_object_core.h"
#include "../unmatched.h"

/* Private helpers of the same duel action controller,
   DuelScene_UpdateHandActions:
   selection-side setup in its state-3 paths followed by execution of the
   AI-script hand/field selection in state 0. */

void func_8001B938(DuelSelectionRecord *selection) {
    DisplayObject *r;
    DuelCardRecord *e;
    u8 *b;
    DuelCardReplayRecordBlock *g;
    s32 k;
    s32 c;
    s32 i;

    D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F48 +
                                       D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE);
    D_8009B1B4->status = 0;
    D_8009B1B4->field_13 = 1;
    D_8009B1B4->field_0C = 0x74;
    D_8009B1B4->field_18 = 0;
    D_8009B1B4->field_11 = 2;
    D_8009B1B4->field_12 = 3;

    if (selection->field_15 == 0) {
        b = D_8015C424;
        r = (DisplayObject *)D_800EA030[(s8)selection->field_0E].object;
        g = (DuelCardReplayRecordBlock *)(b +
            r->field_6A * sizeof(DuelCardRecord) +
            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
        i = (gDuel_adwCardStats[((DuelDeckCardRecord *)g->record.data)->id - 1] >>
             CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    k = i;
        if (k >= CARD_TYPE_MAGIC) {
            if (r->field_20.b.field_21 != 0 || k == CARD_TYPE_TRAP) {
                D_8009B1B4->field_11 = 3;
                D_8009B1B4->field_12 = 4;
            }
        }
    }

    D_8009B1B4->row = D_8009B1B4->field_11;
    c = D_800907D8[
        D_8009B1B4->row * DUEL_FIELD_ROW_SIZE +
        D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ];
    e = &D_801A7AD8[c];

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++, e++) {
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) == 0) {
            D_8009B1B4->col = i;
            break;
        }
    }

    D_8009B162 = 3;
}

/* The deck records sit 0x31E0 bytes below the active-card table, and retail
   forms their base from that table's address rather than from a label of its
   own. */
#define DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS \
    ((DuelDeckCardRecord *)((u8 *)gDuel_aActiveCards - 0x31E0))

/* Executes the AI selection prepared in D_800EAE88: clears selected hand
 * slots, swaps chosen active-card deck records into those slots, respawns the
 * hand objects, and publishes the resulting hand indices.
 *
 * The slot search is a goto loop inside a one-pass do/while, and the order
 * and card-record bases are added as integers: those forms keep the retail
 * loop-invariant placement, operand order and register roles. */
void func_8001BAF0(void)
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
        sel[i] = D_8009B1C8->hand[i];
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = D_800EAE88[i];
        if (v == 0) {
            break;
        }
        if (v < 0x10) {
            sel[v - 0xB] = -1;
        }
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = D_800EAE88[i];
        if (v == 0) {
            return;
        }
        if (v >= 0x10) {
            do {
                j = 0;
                order = D_800907CC;
                records = D_801A7AD8;
                slot = D_800EA030;
            search:
                if (sel[j] >= 0) {
                    card = &gDuel_aActiveCards[v];
                    rec = DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS + sel[j];
                    other = DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS +
                            card->deck_index;
                    v = (s8)rec->index_02;
                    rec->index_02 = other->index_02;
                    other->index_02 = v;
                    tmp = *rec;
                    *rec = *other;
                    *other = tmp;
                    k = D_8009B1D5 * HAND_SIZE;
                    id = *(u8 *)((j + k) + (s32)order);
                    spawned = (DuelCardDisplayObject *)slot->object;
                    Duel_SetupCardRecord(id, (s8)rec->index_02);
                    slot->object = (u8 *)func_80018004(
                        (DuelCardRecord *)(id * DUEL_CARD_RECORD_SIZE +
                                           (s32)records),
                        spawned->out_x, spawned->out_y);
                    DisplayObject_ReleaseIfPresent(spawned);
                    D_8009B1C8->hand[j] = rec->index_02;
                    D_800EAE88[i] = j + 0xB;
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
