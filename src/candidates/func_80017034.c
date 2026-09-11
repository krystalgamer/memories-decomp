/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_80017034.c.
 */
#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "../game/duel_grid.h"
#include "../game/duel_selection_layout.h"
#include "../game/duel_card.h"
#include "../game/func_80017034.h"
#include "../game/input.h"

int func_80017034(DuelCardRecord *argument)
{
    register DuelCardRecord *object asm("$6") = argument;
    register DuelFieldCursor *position asm("$4") =
        (DuelFieldCursor *)D_8009B1B4;
    register u8 *table asm("$3") = D_800907D8;
    int valid = 1;
    unsigned int value =
        table[position->row * DUEL_FIELD_ROW_SIZE + position->col];
    if (value >= DUEL_FIELD_SIDE_GRID_SLOT_COUNT) {
        int mask = object->flags & DUEL_CARD_FLAG_FACE_DOWN;
        valid = (unsigned int)mask < 1;
    }
    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_TRIANGLE) &&
        (object->flags & DUEL_CARD_FLAG_OCCUPIED) && valid)
        return object->card_id;
    return 0;
}
