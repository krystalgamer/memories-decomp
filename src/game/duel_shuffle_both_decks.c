#include "../types.h"
#include "card_constants.h"
#include "duel_shuffle_deck.h"
#include "duel_shuffle_both_decks.h"

extern char gDuel_awPlayerShuffledDeck[];

void Duel_ShuffleBothDecks(void *arg0, void *arg1)
{
    char *base = gDuel_awPlayerShuffledDeck;

    Duel_ShuffleDeck((s32)arg0, (u8 *)base, (u8 *)(base - 0x54));
    Duel_ShuffleDeck((s32)arg1, (u8 *)(base + DECK_SIZE * sizeof(u16)),
                     (u8 *)(base - 0x2C));
}
