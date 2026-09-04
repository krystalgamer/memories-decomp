#include "../types.h"
#include "mem_card.h"

extern int func_8008E680(void *, int);

int func_80044598(int value, char *entry, int count)
{
    int i;
    for (i = 0; i < count; i++, entry += MEM_CARD_DIRECTORY_ENTRY_SIZE) {
        if (func_8008E680(entry, value) == 0)
            return i;
    }
    return -1;
}
