#include "../types.h"
#include "card_list_text_boxes.h"
#include "func_80033500.h"

s32 func_80033500(CardList *list)
{
    s32 index = list->first + list->cursor;
    CardEntry *entry = &list->entries[index];
    if (entry->flags == 0) {
        return 0;
    }
    return *(s16 *)&entry->id;
}
