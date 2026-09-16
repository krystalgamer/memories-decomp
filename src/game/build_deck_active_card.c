#include "../types.h"
#include "card_list_text_boxes.h"
#include "build_deck_active_card.h"

s32 BuildDeck_GetActiveCardID(CardList *list)
{
    s32 index = list->first + list->cursor;
    CardEntry *entry = &list->entries[index];
    if (entry->flags == 0) {
        return 0;
    }
    return (s16)entry->id;
}
