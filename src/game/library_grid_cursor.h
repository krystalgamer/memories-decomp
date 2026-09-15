#ifndef MEMORIES_DECOMP_LIBRARY_GRID_CURSOR_H
#define MEMORIES_DECOMP_LIBRARY_GRID_CURSOR_H

#include "../types.h"

/* Returns the card id under the current Library grid cursor. The state pointer
 * is part of the caller ABI but is not read by the calculation. */
s32 Library_GetGridCursorCardId(u8 *state);

/* Updates Library grid selection, movement, cancellation and scroll state. */
void Library_UpdateGridCursor(u8 *state);

#endif
