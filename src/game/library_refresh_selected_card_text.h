#ifndef MEMORIES_DECOMP_LIBRARY_REFRESH_SELECTED_CARD_TEXT_H
#define MEMORIES_DECOMP_LIBRARY_REFRESH_SELECTED_CARD_TEXT_H

#include "../types.h"
#include "library_state.h"

/* Refreshes text-box slot 1 from the cursor-selected card. Clearing the
 * published selection for an unknown card does not clear the staged ID or
 * selector 5. The existing color_index_54 byte view, unowned override and blocking
 * channel build remain unchanged. The motion-completion caller passes its
 * enclosing LibraryState through the first-member address. */
void Library_RefreshSelectedCardText(const LibraryState *state);

#endif
