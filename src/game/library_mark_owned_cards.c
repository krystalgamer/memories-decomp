#include "../types.h"
#include "card_constants.h"

extern u8 gLibrary_abCardChest[];
extern u16 gDuel_awPlayerDeck[];
extern void Library_UpdateCardUsedFlag();

void Library_MarkOwnedCards(void)
{
    u8 *p = gLibrary_abCardChest;
    s32 i = 0;
    u16 *q;
    do {
        if (*p != 0)
            Library_UpdateCardUsedFlag(i + 0x121);
        i++;
        p++;
    } while (i < CARD_COUNT);
    q = gDuel_awPlayerDeck;
    i = 0;
    do {
        if (*q != 0)
            Library_UpdateCardUsedFlag(*q + 0x120);
        i++;
        q++;
    } while (i < DECK_SIZE);
}
