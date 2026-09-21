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
