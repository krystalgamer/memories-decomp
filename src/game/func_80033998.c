#include "../types.h"
#include "card_constants.h"

extern u8 *D_8009B2FC;
int func_80033998(void)
{
    int index;
    u8 *entry = D_8009B2FC + 0x2D50;
    for (index = 0; index < DECK_SIZE; index++, entry += 0x10) {
        if (entry[0xD] == 0) {
            return 1;
        }
    }
    return 0;
}
