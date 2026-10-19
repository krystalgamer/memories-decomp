#ifndef MEMORIES_DECOMP_CARD_LIST_SORT_H
#define MEMORIES_DECOMP_CARD_LIST_SORT_H

#include "../types.h"
#include "card_list_text_boxes.h"

/* Both break equal unsigned keys with ascending name keys. Equal name keys
 * still return 1, as in retail; these are not inverse total comparators. */
s32 BuildDeck_CompareSortKeyDescending(CardEntry *left, CardEntry *right);
s32 BuildDeck_CompareSortKeyAscending(CardEntry *left, CardEntry *right);

/* Re-sorts the shared sixteen-byte rows in place, then rebuilds the page. */
void BuildDeck_SortCardList(CardList *list);

/* Handles scrolling, page changes and sort selection for one card list. */
s32 BuildDeck_UpdateCardListInput(CardList *list);

#endif
