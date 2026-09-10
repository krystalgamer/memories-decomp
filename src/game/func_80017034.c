#define GINPUT_PAD1_PRESSED_SIZED5
#include "../types.h"
#include "duel_grid.h"
#include "duel_card.h"
#include "func_80017034.h"
#include "input.h"

extern DuelFieldCursor *D_8009B1B4;

int func_80017034(DuelCardRecord *argument)
{
    register DuelCardRecord *object asm("$6") = argument;
    register DuelFieldCursor *position asm("$4") = D_8009B1B4;
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
