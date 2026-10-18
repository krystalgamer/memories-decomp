#define GINPUT_PAD1_PRESSED_IN_DATA
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define MAIN_MODE_STATE_NEXT_AS_ARRAY
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "../unmatched.h"
#include "input.h"
#include "card_constants.h"
#include "card_grid.h"
#include "func_8002A3CC.h"
#include "func_80029EB0.h"
#include "library_update_grid_scroll.h"
#include "library_grid_cursor.h"
#include "main_mode_state.h"

s32 Library_GetGridCursorCardId(const LibraryState *state)
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

#ifndef LIBRARY_GRID_CONFIRM_BUTTON
#define LIBRARY_GRID_CONFIRM_BUTTON PAD_BUTTON_CROSS
#endif

#ifndef LIBRARY_GRID_CANCEL_BUTTON
#define LIBRARY_GRID_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif

/*
 * State 1 of the Library screen. The held-input view remains volatile because
 * every direction path must retain the retail input reloads.
 */
void Library_UpdateGridCursor(LibraryState *state)
{
    s32 card_id;
    s32 column;
    s32 row;
    s32 frames;
    s32 distance;

    if (Library_UpdateCursorMotion() == 0) {
        if ((gInput_wPad1Pressed & LIBRARY_GRID_CONFIRM_BUTTON) != 0) {
            card_id = Library_GetGridCursorCardId(state);
            if ((Library_GetCardFlags(state, card_id) &
                 LIBRARY_CARD_FLAG_KNOWN) != 0) {
                state->motion.selected_card_id = card_id;
                state->motion.state = 2;
                return;
            }
        }
        if ((gInput_wPad1Pressed & LIBRARY_GRID_CANCEL_BUTTON) != 0) {
            D_8009B26C[0] = D_8009B269[0];
            return;
        }
        if ((gInput_wPad1Held & (PAD_DIRECTION_MASK | PAD_BUTTON_L1_R1_MASK)) != 0) {
            column = gCardGrid_bCursorColumn;
            row = gCardGrid_bCursorRow;
            frames = 6;
            if ((gInput_wPad1Held & PAD_BUTTON_SQUARE) != 0) {
                frames = 2;
            }
            if ((gInput_wPad1Held & PAD_BUTTON_L1_R1_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_R1) != 0) {
                    row += 0xA;
                    if (row >= 0x28) {
                        row = 0x27;
                    }
                } else {
                    row -= 0xA;
                    if (row < 0) {
                        row = 0;
                    }
                }
                distance = row - gCardGrid_bCursorRow;
                if (distance >= 0) {
                    frames = distance * 2;
                } else {
                    frames = (gCardGrid_bCursorRow - row) * 2;
                }
            } else {
                if ((gInput_wPad1Held & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                    if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
                        column += 1;
                        if (column >= 0x14) {
                            column = 0x13;
                            if (row < 0x27) {
                                column = 0;
                                row += 1;
                                goto wrap_delay;
                            }
                        }
                    } else {
                        column -= 1;
                        if (column < 0) {
                            column = 0;
                            if (row != 0) {
                                column = 0x13;
                                row -= 1;
wrap_delay:
                                frames *= 4;
                            }
                        }
                    }
                }
                if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                    if ((gInput_wPad1Held & PAD_DIRECTION_DOWN) != 0) {
                        row += 1;
                        if (row >= 0x28) {
                            row = 0x27;
                        }
                    } else {
                        row -= 1;
                        if (row < 0) {
                            row = 0;
                        }
                    }
                }
            }
            Library_StartCursorMotion(column, row, frames);
        }
    }
    Library_UpdateGridScroll(&state->motion);
}
