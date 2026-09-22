#include "../types.h"
#include "save_data.h"
#include "card_constants.h"
#include "duel_deck_lookup.h"

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FIND_PLAYER_DECK_CARD)
int Duel_FindPlayerDeckCard(int card_id)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == card_id) {
            return i;
        }
        i++;
        entry++;
    }
    return -1;
}
#endif

/* Any copy in the trunk returns 1 at once; otherwise the deck slot decides. */
#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_LIBRARY_CHECK_CARD_OWNED)
int Library_CheckCardOwned(int card_id)
{
    int flag;

    flag = (((SaveDataWorkspace *)D_801D0000)->state.card_quantities[
        card_id - CARD_ID_FIRST] != 0) ? 1 : -1;
    if (flag < 0) {
        return Duel_FindPlayerDeckCard(card_id);
    }
    return 1;
}
#endif
