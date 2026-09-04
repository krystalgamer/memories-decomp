#include "../types.h"
#include "mem_card.h"

int func_80044544(char *entry, int count)
{
    int i;
    int total = 0;
    for (i = 0; i < count; i++, entry += MEM_CARD_DIRECTORY_ENTRY_SIZE) {
        int value = *(int *)(entry + 24);
        total += value / MEM_CARD_BLOCK_SIZE;
        if (value % MEM_CARD_BLOCK_SIZE)
            total++;
    }
    return MEM_CARD_BLOCK_COUNT - total;
}
