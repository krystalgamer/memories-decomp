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
#ifdef VERSION_JAPAN
    /* The Japanese build asks only that the card be occupied and carry no
       display marker (flags 0x8000 set, 0x2000 clear), read as the word that
       ends at flags, as Duel_UpdateCardDisplayMarker does. The US build instead reads the
       cursor's grid cell and rejects a face-down card when that cell's value
       is DUEL_FIELD_SIDE_GRID_SLOT_COUNT or more. */
    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_TRIANGLE) &&
        (*(s32 *)&card->terrain_modifier & 0xA0000000) == 0x80000000)
        return card->card_id;
    return 0;
#else
    DuelSelectionRecord *position = gDuel_pActiveSelectionRecord;
    s32 valid;
    u32 value =
        gDuel_abFieldGridCardRecordIndices[position->row * DUEL_FIELD_ROW_SIZE + position->col];

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
#endif
}
