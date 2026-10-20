#include "../types.h"
#include "library_get_card_flags.h"

u32 Library_GetCardFlags(const LibraryState *base, s32 index)
{
    return base->cards[index].flags;
}
