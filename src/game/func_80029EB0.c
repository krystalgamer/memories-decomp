#include "../types.h"
#include "func_80029EB0.h"

u32 Library_GetCardFlags(const LibraryState *base, s32 index)
{
    return base->cards[index].flags;
}
