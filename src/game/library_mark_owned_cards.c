#include "../types.h"
#include "save_data.h"
#include "card_constants.h"
#include "campaign_flags.h"

extern u8 gLibrary_abCardChest[];

void Library_MarkOwnedCards(void)
{
    u8 *p = gLibrary_abCardChest;
    s32 i = 0;
    u16 *q;
    do {
        if (*p != 0)
            Library_UpdateCardUsedFlag(i + (CAMPAIGN_FLAG_LIBRARY_CARD_BASE + CARD_ID_FIRST));
        i++;
        p++;
    } while (i < CARD_COUNT);
    q = gDuel_awPlayerDeck;
    i = 0;
    do {
        if (*q != 0)
            Library_UpdateCardUsedFlag(*q + CAMPAIGN_FLAG_LIBRARY_CARD_BASE);
        i++;
        q++;
    } while (i < DECK_SIZE);
}
