#include "../types.h"
#include "sorted_entry_compare.h"

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
