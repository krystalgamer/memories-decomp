#include "../../types.h"
#define SORTED_ENTRY_RELINK_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C26C-0x8009C280 (#2602); its
 * layout differs from the North American block the shared source defines. */
u32 gModel_dwFogDrawFlags;
struct SortedEntry *gSortedEntry_pCursor;
u32 gSortedEntry_nCount;
struct SortedEntry *gSortedEntry_pBase;
u32 gSortedEntry_nSortedCount;

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../sorted_entry_relink.c"
