#include "../types.h"
#include "sorted_entry.h"

/* Zero-initialized storage this unit owns at 0x8009B304-0x8009B318, defined in
 * address order ahead of the headers that declare it (#2602). */
SortedEntry *G32 gSortedEntry_pBase;
u32 gSortedEntry_nSortedCount;
u32 gModel_dwFogDrawFlags;
SortedEntry *G32 gSortedEntry_pCursor;
u32 gSortedEntry_nCount;

#include "../psyq/qsort.h"

/* Three-way comparator over the first word of two sorted-entry records,
 * returning 0, 1 or -1. SortedEntry_SortAndRelink below is its only consumer
 * and hands it to qsort over eight-byte entries; nothing calls it directly. */
int SortedEntry_Compare(const u32 *left, const u32 *right)
{
    if (*left == *right) {
        return 0;
    }
    if (*left >= *right) {
        return 1;
    }
    return -1;
}

/* Use the historical PSYQ declaration, whose comparator has unspecified args. */
void SortedEntry_SortAndRelink(void)
{
    SortedEntry *base = gSortedEntry_pBase;
    SortedEntry *entry = base;
    u32 count = gSortedEntry_nCount;
    u32 sorted_count;
    u32 index;

    gSortedEntry_nSortedCount = count;
    gModel_dwFogDrawFlags &= ~4;

    if (count >= 2) {
        s32 (*compare)(const u32 *, const u32 *) = SortedEntry_Compare;

        qsort(entry, count, sizeof(*entry), compare);
    }

    sorted_count = gSortedEntry_nSortedCount;
    index = 0;

    if (sorted_count != 0) {
        do {
            base[entry->append_index].sorted_position = index;
            index++;
            entry++;
        } while (index < sorted_count);
    }

    gSortedEntry_nCount = 0;
    gSortedEntry_pCursor = gSortedEntry_pBase;
}
