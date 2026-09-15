#ifndef MEMORIES_DECOMP_BUILD_DECK_ACTIVE_CARD_H
#define MEMORIES_DECOMP_BUILD_DECK_ACTIVE_CARD_H

#include "card_list_text_boxes.h"

/* Returns the id of the entry the cursor is on, or 0 when that row's flag
   byte is clear. */
s32 BuildDeck_GetActiveCardID(CardList *list);

#endif
