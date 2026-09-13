#ifndef MEMORIES_DECOMP_CARD_LIST_SORT_H
#define MEMORIES_DECOMP_CARD_LIST_SORT_H

#include "../types.h"
#include "card_list_text_boxes.h"

/* One 16-byte row sorted in place. The comparators consume the key and card
 * id; func_80032C48 also uses field_0D to exclude unavailable rows. */
typedef struct {
    u32 key;
    s16 card_id;
    u8 pad_06[7];
    u8 field_0D;
    u8 pad_0E[2];
} CardListSortItem;

s32 BuildDeck_CompareCard(
    CardListSortItem *left,
    CardListSortItem *right
);
s32 func_80032BD4(CardListSortItem *left, CardListSortItem *right);

/* Re-sorts the list's rows in place. The rows are the same sixteen bytes
 * CardList spells as CardEntry; this file names them CardListSortItem
 * because it reads the key word at +0 that the text-box side never touches. */
void func_80032C48(CardList *list);

/* Handles scrolling, page changes and sort selection for one card list. */
s32 func_800330BC(CardList *list);

#endif
