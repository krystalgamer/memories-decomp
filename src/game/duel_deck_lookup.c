#include "../types.h"
#include "save_data.h"
#include "card_constants.h"
#include "duel_deck_lookup.h"

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

/* A set flag returns success immediately; otherwise use the card's deck index. */
int func_8002C518(int a0)
{
    int flag;

    flag = (((SaveDataWorkspace *)D_801D0000)->state.card_quantities[
        a0 - CARD_ID_FIRST] != 0) ? 1 : -1;
    if (flag < 0) {
        return Duel_FindPlayerDeckCard(a0);
    }
    return 1;
}
