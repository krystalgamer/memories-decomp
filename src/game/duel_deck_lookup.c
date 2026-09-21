#include "../types.h"
#include "save_data.h"
#include "card_constants.h"
#include "duel_deck_lookup.h"

/* Any copy in the trunk returns 1 at once; otherwise the deck slot decides. */
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
