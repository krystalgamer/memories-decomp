#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "func_80017034.h"
#include "input.h"

s32 func_80017034(DuelCardRecord *card)
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
