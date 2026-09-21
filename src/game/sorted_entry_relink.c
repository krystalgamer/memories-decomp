#include "../types.h"
#include "sorted_entry.h"
#include "sorted_entry_compare.h"
#include "../psyq/qsort.h"

/* Use the historical PSYQ declaration, whose comparator has unspecified args. */
void SortedEntry_SortAndRelink(void)
{
    SortedEntry *base = D_8009B304;
    SortedEntry *entry = base;
    u32 count = D_8009B314;
    u32 sorted_count;
    u32 index;

    D_8009B308 = count;
    D_8009B30C &= ~4;

    if (count >= 2) {
        s32 (*compare)(const u32 *, const u32 *) = SortedEntry_Compare;

        qsort(entry, count, sizeof(*entry), compare);
    }

    sorted_count = D_8009B308;
    index = 0;

    if (sorted_count != 0) {
        do {
            base[entry->append_index].sorted_position = index;
            index++;
            entry++;
        } while (index < sorted_count);
    }

    D_8009B314 = 0;
    D_8009B310 = D_8009B304;
}
