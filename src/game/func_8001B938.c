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
#include "display_object_api.h"
#include "../unmatched.h"

/* Private helper of the duel action controller, func_8001BD88: selection-side
   setup in its state-3 paths. The helper after it, which executes the
   AI-script hand/field selection in state 0, is func_8001BAF0, now a
   candidate in src/candidates/func_8001BAF0.c. */

void func_8001B938(u8 *p) {
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

    if (p[0x15] == 0) {
        b = D_8015C424;
        r = (DisplayObject *)D_800EA030[*(s8 *)(p + 0xE)].object;
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

