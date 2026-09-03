#include "../types.h"
#include "card_constants.h"

extern unsigned short gDuel_awPlayerDeck[];

int func_8002EE5C(void)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == 0) {
            return 0;
        }
        i++;
        entry++;
    }
    return 1;
}
