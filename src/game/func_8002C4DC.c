#include "../types.h"
#include "card_constants.h"

extern unsigned short gDuel_awPlayerDeck[];

int func_8002C4DC(int value)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == value) {
            return i;
        }
        i++;
        entry++;
    }
    return -1;
}
