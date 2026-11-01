#define D_8009B360_IN_DATA
#include "../types.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "ai_opponent_data.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "duel_card.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"

#define DUEL_SELECTION_RECORDS(address) ((DuelSelectionRecord *)(address))

void Duel_InitSideStates(void) {
    u16 sp[DUEL_SIDE_COUNT];
    DuelSideState *e;
    s32 k;
    s32 j;
    u32 m;
    u16 t;
    u8 *q;
    s8 *r;

    e = gDuel_aSideStates;
    if (gDuel_bOpponentID < 0) {
        sp[0] = gDuel_wTwoPlayerSide0LifePoints;
        sp[1] = gDuel_wTwoPlayerSide1LifePoints;
    } else {
        sp[1] = DUEL_STARTING_LIFE_POINTS;
        sp[0] = DUEL_STARTING_LIFE_POINTS;
    }

    for (k = 0; k < DUEL_SIDE_COUNT; k++, e++) {
        j = 4;
        r = (s8 *)e + j;
        /* Preserve the backward cursor while deriving the hand-array base. */
        for (; j >= 0; j--, r--) {
            r[(u32)&((DuelSideState *)0)->hand] = -1;
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

    if (gDuel_bSide0ControllerSelector < 0) {
        if (gDuel_bOpponentID < 0) {
            gDuel_aSideStates[1].card_view_mode = *(u8 *)&gDuel_wTwoPlayerCardViewMode;
            gDuel_aSideStates[0].card_view_mode = *(u8 *)&gDuel_wTwoPlayerCardViewMode;
        } else {
            gDuel_bSide1CardViewMode[0] = -1;
        }
    } else {
        if (gDuel_bOpponentID >= 0) {
            gDuel_bSide1CardViewMode[0] = -1;
        }
    }
    gDuel_pActiveSideState = &gDuel_aSideStates[gDuel_bActiveSide];
}

void Duel_ClearHandSlots(void)
{
    u8 *entry = (u8 *)gDuelHand_aSlots;
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

void Duel_InitSelectionRecords(void) {
    s32 row, j;
    for (row = 0; row < DUEL_SIDE_COUNT; row++) {
        DuelSelectionRecord *p = DUEL_SELECTION_RECORDS(
            gDuel_abSelectionSides + row * DUEL_SELECTION_SIDE_SIZE);
        for (j = 0; j < DUEL_SELECTION_RECORDS_PER_SIDE; p++, j++) {
            p->field_00 = 0;
            p->cursor_object = 0;
            p->hand = (DuelHandSlot *)0;
            p->field_18 = 0;
            p->field_13 = 1;
            p->record_index = j;
#ifdef VERSION_JAPAN
            p->field_14 = j;
#else
            p->field_14 = (j != 3) ? j : 1;
#endif
        }
    }
    DUEL_SELECTION_RECORDS(gDuel_abSelectionSides)[0].field_13 = 0;
    DUEL_SELECTION_RECORDS(gDuel_abSelectionSides)[DUEL_SELECTION_RECORDS_PER_SIDE]
        .field_13 = 0;
}

void Duel_ResetCardRecords(void)
{
    DuelCardRecord *entry = gDuel_aCardRecords;
    int i = 0;
    DuelCardFlagsCursor *flags = (DuelCardFlagsCursor *)&entry->flags;

    do {
        *(s32 *)&entry->object = 0;
        /* Keep the flags-relative cursor while deriving the data distance. */
        *(s32 *)(&flags->flags -
            ((u32)&((DuelCardRecord *)0)->flags -
             (u32)&((DuelCardRecord *)0)->data) / sizeof(flags->flags)) = 0;
        flags->flags = 0;
        flags++;
        i++;
        entry++;
    } while (i < DUEL_CARD_RECORD_COUNT);
}
