#include "../types.h"
#include "sorted_entry.h"

void func_80035668(u32 value)
{
    D_8009B30C = value;
    D_8009B300 = 0x808080;
}

void func_80035680(u32 value)
{
    D_8009B314 = 0;
    D_8009B310 = (SortedEntry *)value;
    D_8009B304 = (SortedEntry *)value;
    D_8009B30C |= 4;
}
