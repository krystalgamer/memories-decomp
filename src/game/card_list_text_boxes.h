#ifndef MEMORIES_DECOMP_CARD_LIST_TEXT_BOXES_H
#define MEMORIES_DECOMP_CARD_LIST_TEXT_BOXES_H

#include "../types.h"
#include "card_constants.h"

/* One row of the card list: the entry's id and the flag that selects the
 * highlighted style. The gaps are what these two functions do not read, not
 * a claim that the row is otherwise unused. */
typedef struct {
    u8 pad_00[4];
    u16 id;
    u8 pad_06[7];
    u8 flags;
    u8 pad_0E[2];
} CardEntry;

/* The card list itself: CARD_ID_END rows, then the scroll offset the slot is
 * measured from and the list kind that picks the box template. */
typedef struct {
    CardEntry entries[CARD_ID_END];
    u8 pad_2D30[0xC];
    s16 first;
    u8 pad_2D3E[9];
    u8 kind;
} CardList;

void func_80031CD4(CardList *list, s32 slot);
void func_80031E04(CardList *list, s32 count);

#endif
