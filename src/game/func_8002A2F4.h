#ifndef MEMORIES_DECOMP_FUNC_8002A2F4_H
#define MEMORIES_DECOMP_FUNC_8002A2F4_H

#include "../types.h"
#include "library_state.h"

/* Stages the selected card for text request 1 and opens its text box.
 * Library_GetGridCursorCardId's result goes to gDuel_wSelectedCardID and the
 * text staging record. A non-zero id selects text mode 5, and the selection
 * is cleared again unless the id's flags byte in `state`
 * (Library_GetCardFlags) has
 * bit 0x80. D_8009B320 starts from the first byte of the created display
 * object's field_54 pointer, and bit 0 of the state byte forces it to 4.
 * Library_UpdateCursorMotion passes the enclosing LibraryState at its motion prefix's
 * address. */
void func_8002A2F4(const LibraryState *state);

#endif
