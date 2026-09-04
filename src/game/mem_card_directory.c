#include "../types.h"

#include "mem_card.h"

extern s32 func_8008E680(void *, s32);

s32 func_80044544(u8 *entry, s32 count)
{
    s32 i;
    s32 total = 0;

    for (i = 0; i < count; i++, entry += MEM_CARD_DIRECTORY_ENTRY_SIZE) {
        s32 value = *(s32 *)(entry + 24);

        total += value / MEM_CARD_BLOCK_SIZE;
        if (value % MEM_CARD_BLOCK_SIZE) {
            total++;
        }
    }
    return MEM_CARD_BLOCK_COUNT - total;
}

s32 func_80044598(s32 value, u8 *entry, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++, entry += MEM_CARD_DIRECTORY_ENTRY_SIZE) {
        if (func_8008E680(entry, value) == 0) {
            return i;
        }
    }
    return -1;
}
