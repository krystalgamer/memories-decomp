#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "card_list_sort.h"

void func_80031EE4(u8 *base, s32 index)
{
    u8 *counts = base + index;
    u32 raw = counts[0x5D97];
    s32 count = raw & 255;

    if (count == 0) {
        CardListSortItem *entry = (CardListSortItem *)(base + 4);

        (*(s32 *)(base + 0x5A9C))++;
        counts[0x5D97]++;
        do {
            s32 id = entry->card_id;

            entry++;
            if (id == index)
                break;
        } while (1);
        entry--;
        /* Keep the post-search adjustment separate from the flag store. */
        ((volatile CardListSortItem *)entry)->field_0D = 1;
        func_80032C48((CardList *)(base + 4));
    } else if (count != CARD_CHEST_QUANTITY_MAX) {
        s32 next = raw + 1;

        counts[0x5D97] = next;
        (*(s32 *)(base + 0x5A9C))++;
    }
}
