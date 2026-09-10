#define FUNC_80018004_AMBIENT_POSITION_ARGS
#include "../types.h"
#include "duel_deck_card.h"
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
#include "display_object_api.h"
#include "../unmatched.h"

extern u8 D_8015C424[];
extern u8 D_800EAE88[];

/* Private helpers of the same duel action controller, func_8001BD88:
   selection-side setup in its state-3 paths followed by execution of the
   AI-script hand/field selection in state 0. */

void func_8001B938(u8 *p) {
    DisplayObject *r;
    DuelCardRecord *e;
    u8 *b;
    u8 *g;
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

    if (p[0x15] == 0) {
        b = D_8015C424;
        r = (DisplayObject *)D_800EA030[*(s8 *)(p + 0xE)].object;
        g = b + r->field_6A * DUEL_CARD_RECORD_SIZE + 0x48000;
        i = (gDuel_adwCardStats[*(s16 *)*(s32 *)(g + 0x36B8) - 1] >>
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

/* Executes the AI selection prepared in D_800EAE88: clears selected hand
 * slots, swaps chosen active-card deck records into those slots, respawns the
 * hand objects, and publishes the resulting hand indices. */
void func_8001BAF0(void)
{
    s8 sel[HAND_SIZE];
    DuelDeckCardRecord tmp;
    register u8 *hand asm("$22");
    u8 *p;
    u8 *end;
    s8 *q;
    u8 *deck;
    register u8 *base asm("$8");
    u8 *tbl;
    u8 *recs;
    register DuelDeckCardRecord *rec asm("$16");
    DuelDeckCardRecord *other;
    DuelHandSlot *slot;
    DuelCardDisplayObject *spawned;
    u8 *card;
    s32 i;
    s32 j;
    register s32 v asm("$5");
    s32 id;
    s32 sidx;
    register s32 a asm("$5");
    s32 b;

    for (i = 0; i < HAND_SIZE; i++) {
        p = (u8 *)D_8009B1C8;
        sel[i] = *(u8 *)((0x1A + i) + (s32)p);
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = D_800EAE88[i];
        if (v == 0) {
            break;
        }
        if (v < 0x10) {
            p = (u8 *)&sel[v - 0xB];
            *(s8 *)p = -1;
        }
    }
    base = (u8 *)gDuel_aActiveCards;
    deck = base - 0x31E0;
    hand = D_800EAE88;
    end = D_800EAE88;
next:
    v = *hand;
    if (v == 0) {
        return;
    }
    if (v >= 0x10) {
        j = 0;
        tbl = D_800907CC;
        recs = (u8 *)D_801A7AD8;
        slot = D_800EA030;
    inner:
        q = &sel[j];
        sidx = *q;
        if (sidx >= 0) {
            {
                s32 o;

                o = v * AI_ACTIVE_CARD_RECORD_SIZE;
                {
                    register u8 *act asm("$9") = (u8 *)gDuel_aActiveCards;

                    card = act + o;
                }
            }
            rec = (DuelDeckCardRecord *)(sidx * 6 + (s32)deck);
            other = (DuelDeckCardRecord *)(card[0xB] * 6 + (s32)deck);
            a = *(s8 *)&rec->index_02;
            b = other->index_02;
            rec->index_02 = b;
            other->index_02 = a;
            tmp = *rec;
            *rec = *other;
            *other = tmp;
            {
                s32 k;

                k = D_8009B1D5 * HAND_SIZE;
                id = *(u8 *)((j + k) + (s32)tbl);
            }
            spawned = (DuelCardDisplayObject *)slot->object;
            Duel_SetupCardRecord(id, *(s8 *)&rec->index_02);
            slot->object = (u8 *)func_80018004(
                (DuelCardRecord *)(id * DUEL_CARD_RECORD_SIZE + (s32)recs),
                spawned->out_x,
                spawned->out_y
            );
            func_8004036C(spawned);
            *(u8 *)((s32)D_8009B1C8 + (0x1A + j)) = rec->index_02;
            *hand = j + 0xB;
            *q = -1;
        } else {
            j++;
            slot++;
            if (j < HAND_SIZE) {
                goto inner;
            }
        }
    }
    hand++;
    if ((s32)hand < (s32)(end + HAND_SIZE)) {
        goto next;
    }
}
