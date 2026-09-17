#ifndef MEMORIES_DECOMP_LIBRARY_GRID_CURSOR_H
#define MEMORIES_DECOMP_LIBRARY_GRID_CURSOR_H

#include "../types.h"

/* Prefix of the Library screen state shared by the grid-selection handler and
 * the state-2 card viewer. The latter repeatedly uses +0x06 as the selected
 * card ID for card setup, stats, and marker indexing. */
typedef struct {
    u8 pad_00[6];
    s16 selected_card_id;
} LibrarySelectionState;

/* Returns the card id under the current Library grid cursor. The state pointer
 * is part of the caller ABI but is not read by the calculation. */
s32 Library_GetGridCursorCardId(u8 *state);

/* Updates Library grid selection, movement, cancellation and scroll state. */
void Library_UpdateGridCursor(u8 *state);

#endif
