#include "../types.h"
#include "sorted_entry.h"
#include "text_render_state.h"

void func_80035668(u32 value)
{
    gModel_dwFogDrawFlags = value;
    gModel_dwFogBaseColor = 0x808080;
}

void SortedEntry_BeginCollection(SortedEntry *list)
{
    gSortedEntry_nCount = 0;
    gSortedEntry_pCursor = list;
    gSortedEntry_pBase = list;
    gModel_dwFogDrawFlags |= 4;
}
