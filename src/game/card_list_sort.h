#ifndef MEMORIES_DECOMP_CARD_LIST_SORT_H
#define MEMORIES_DECOMP_CARD_LIST_SORT_H

#include "../types.h"

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

/* Re-sorts the card list that starts at the given record array. */
void func_80032C48(u8 *p);

#endif
