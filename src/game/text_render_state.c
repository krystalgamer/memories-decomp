#include "../types.h"
#include "sorted_entry.h"
#include "text_render_state.h"

void func_80035668(u32 value)
{
    D_8009B30C = value;
    D_8009B300 = 0x808080;
}

void SortedEntry_BeginCollection(SortedEntry *list)
{
    D_8009B314 = 0;
    D_8009B310 = list;
    D_8009B304 = list;
    D_8009B30C |= 4;
}
