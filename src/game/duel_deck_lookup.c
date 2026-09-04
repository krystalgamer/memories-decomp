#include "../types.h"
#include "card_constants.h"

extern unsigned short gDuel_awPlayerDeck[];
extern u8 D_801D0000[];

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

/* A set flag returns success immediately; otherwise use the card's deck index. */
int func_8002C518(int a0)
{
    int flag;

    flag = (D_801D0000[a0 + 591] != 0) ? 1 : -1;
    if (flag < 0) {
        return func_8002C4DC(a0);
    }
    return 1;
}
