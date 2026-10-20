#ifndef MEMORIES_DECOMP_DUEL_CARD_PICK_CURSOR_H
#define MEMORIES_DECOMP_DUEL_CARD_PICK_CURSOR_H

#include "../types.h"
#include "duel_selection_layout.h"

/* The mode byte for the "pick a card off the field" cursor, at 0x2CC($gp).
 * Duel_UpdateCardPickCursor owns it, and its own comment gives the bits:
 *
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until the display request clears
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 *
 * Zero therefore means the cursor is not active at all, which is how
 * Duel_InitScene uses it: it clears this byte in the run of assignments that
 * resets the duel, alongside gDuel_wDisplayTransitionFlags, the request/status
 * word the 0x40 hold waits on rather than a frame countdown.
 *
 * DuelScene_UpdateHandActions is the third accessor; it reads and writes the
 * byte gp-relative like everything else here. */
extern u8 gDuelCursor_PickFlags;

void Duel_UpdateCardPickCursor(DuelSelectionRecord *cursor);

#endif
