#include "../types.h"
#include "../psyq/libgte.h"
#include "sorted_entry.h"

void SortedEntry_Append(s32 dx, s32 dy, s32 dz) {
    SortedEntry *p;
    s32 q;
    s32 v;

    gSortedEntry_pCursor->distance = SquareRoot0(dx * dx + dz * dz);
    q = ratan2(dx, dz) / 16;
    if (q >= 256) {
        q = 255;
    }
    v = q << 8;

    p = gSortedEntry_pCursor;
    p->packed = v | (dy >> 4);
    p->append_index = gSortedEntry_nCount;
    p->sorted_position = 0;

    gSortedEntry_pCursor = p + 1;
    gSortedEntry_nCount = gSortedEntry_nCount + 1;
}
