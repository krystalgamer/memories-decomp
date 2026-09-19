#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "duel_get_card_viewer_request_id.h"
#include "input.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_card_can_act_this_turn.h"

/* The two per-card checks DuelScene_UpdateFieldActions makes on the grid card
   under the cursor: whether it can still act this turn, and whether Triangle
   opens the card viewer for it. */

int DuelCard_CanActThisTurn(DuelCardRecord *object)
{
    u16 flags = object->flags;

    if (flags & DUEL_CARD_FLAG_OCCUPIED) {
        if (!(flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
            return 1;
        }
    }
    return 0;
}

s32 Duel_GetCardViewerRequestId(DuelCardRecord *card)
{
    DuelCardPickCursor *position = D_8009B1B4;
    s32 valid;
    u32 value =
        D_800907D8[position->row * DUEL_FIELD_ROW_SIZE + position->col];

    if (value < DUEL_FIELD_SIDE_GRID_SLOT_COUNT)
        valid = 1;
    else {
        s32 mask = card->flags & DUEL_CARD_FLAG_FACE_DOWN;
        valid = (u32)mask < 1;
    }

    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_TRIANGLE) &&
        (card->flags & DUEL_CARD_FLAG_OCCUPIED) && valid)
        return card->card_id;
    return 0;
}
