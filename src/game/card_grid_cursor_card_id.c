#include "../types.h"
#include "../game/card_constants.h"
#include "../game/card_grid.h"
#include "../game/func_8002A788.h"

s32 func_8002A6B8(void)
{
    s32 row = gCardGrid_bCursorRow;
    s32 row_tens = row / CARD_GRID_SECTION_SIDE_LENGTH;
    s8 row_tens_byte = (s8)row_tens;
    s32 index = row_tens_byte * CARD_GRID_SECTION_ROW_CARD_COUNT;
    s8 row_ones =
        (s8)(row - row_tens * CARD_GRID_SECTION_SIDE_LENGTH);
    s32 base;
    s32 column;
    s32 column_tens;
    s8 column_ones;
    s32 result;

    index += row_ones * CARD_GRID_SECTION_SIDE_LENGTH;

    column = gCardGrid_bCursorColumn;
    if (column >= CARD_GRID_SECTION_SIDE_LENGTH) {
        index += CARD_GRID_SECTION_CARD_COUNT;
    }
    base = index + 1;
    column_tens = column / CARD_GRID_SECTION_SIDE_LENGTH;
    column_ones =
        (s8)(column - column_tens * CARD_GRID_SECTION_SIDE_LENGTH);
    index = base + column_ones;
    if (index >= CARD_ID_END) {
        return 0;
    }
    return index;
}
