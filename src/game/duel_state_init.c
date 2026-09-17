#define D_8009B360_IN_DATA
#include "../types.h"
#include "duel_side_state.h"
#include "ai_opponent_data.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "duel_card.h"
#include "../unmatched.h"

void func_800175A0(void) {
    u16 sp[DUEL_SIDE_COUNT];
    DuelSideState *e;
    s32 k;
    s32 j;
    u32 m;
    u16 t;
    u8 *q;
    s8 *r;

    e = D_800E9FF0;
    if (gDuel_bOpponentID < 0) {
        sp[0] = D_8009B234;
        sp[1] = D_8009B236;
    } else {
        sp[1] = DUEL_STARTING_LIFE_POINTS;
        sp[0] = DUEL_STARTING_LIFE_POINTS;
    }

    for (k = 0; k < DUEL_SIDE_COUNT; k++, e++) {
        j = 4;
        r = (s8 *)e + j;
        for (; j >= 0; j--, r--) {
            r[0x1A] = -1;
        }
        q = (u8 *)e;
        e->deck_draw_cursor = 0;
        t = sp[k];
        e->displayed_life_points = 0;
        e->swords_turns_remaining = 0;
        e->life_points.signed_value = t;
        e->max_life_points = t;
        e->card_view_mode = 0;
        for (m = 0; m < sizeof(DuelRankStatistics); m++) {
            *q = 0;
            q++;
        }
    }

    if (D_8009B360 < 0) {
        if (gDuel_bOpponentID < 0) {
            D_800E9FF0[1].card_view_mode = *(u8 *)&D_8009B230;
            D_800E9FF0[0].card_view_mode = *(u8 *)&D_8009B230;
        } else {
            D_800EA02F[0] = -1;
        }
    } else {
        if (gDuel_bOpponentID >= 0) {
            D_800EA02F[0] = -1;
        }
    }
    D_8009B1C8 = &D_800E9FF0[D_8009B1D5];
}

void Duel_ClearHandSlots(void)
{
    u8 *entry = (u8 *)D_800EA030;
    int i = 0;
    u8 *field_09 = entry + 9;

    do {
        *(int *)(field_09 - 5) = 0;
        *(int *)entry = 0;
        *field_09 = 0;
        field_09 += sizeof(DuelHandSlot);
        i++;
        entry += sizeof(DuelHandSlot);
    } while (i < HAND_SIZE);
}

void func_80017708(void) {
    s32 row, j;
    for (row = 0; row < DUEL_SIDE_COUNT; row++) {
        DuelSelectionRecord *p = (DuelSelectionRecord *)
            (D_800E9F10 + row * DUEL_SELECTION_SIDE_SIZE);
        for (j = 0; j < DUEL_SELECTION_RECORDS_PER_SIDE; p++, j++) {
            p->field_00 = 0;
            p->cursor_object = 0;
            p->hand = (DuelHandSlot *)0;
            p->field_18 = 0;
            p->field_13 = 1;
            p->field_17 = j;
            p->field_14 = (j != 3) ? j : 1;
        }
    }
    ((DuelSelectionRecord *)D_800E9F10)[0].field_13 = 0;
    ((DuelSelectionRecord *)D_800E9F10)[DUEL_SELECTION_RECORDS_PER_SIDE]
        .field_13 = 0;
}

void func_8001778C(void)
{
    DuelCardRecord *entry = D_801A7AD8;
    int i = 0;
    u16 *flags = &entry->flags;

    do {
        *(s32 *)&entry->object = 0;
        /* Keep the flags-relative cursor while deriving the data distance. */
        *(s32 *)(flags -
            ((u32)&((DuelCardRecord *)0)->flags -
             (u32)&((DuelCardRecord *)0)->data) / sizeof(*flags)) = 0;
        *flags = 0;
        flags = (u16 *)((u8 *)flags + sizeof(DuelCardRecord));
        i++;
        entry++;
    } while (i < DUEL_CARD_RECORD_COUNT);
}
