#include "../types.h"
#include "card_constants.h"

extern s8 gCardGrid_bCursorColumn;
extern s8 gCardGrid_bCursorRow;

s32 func_8002A6B8(void)
{
    s32 row = gCardGrid_bCursorRow;
    s32 row_tens = row / 10;
    s8 row_tens_byte = (s8)row_tens;
    s32 index = row_tens_byte * 200;
    s8 row_ones = (s8)(row - row_tens * 10);
    s32 base;
    s32 column;
    s32 column_tens;
    s8 column_ones;
    register s32 result asm("$2");

    index += row_ones * 10;

    column = gCardGrid_bCursorColumn;
    if (column >= 10) {
        index += 100;
    }
    base = index + 1;
    column_tens = column / 10;
    column_ones = (s8)(column - column_tens * 10);
    index = base + column_ones;
    result = 0;
    if (index < CARD_COUNT + 1) {
        result = index;
    }
    return result;
}
