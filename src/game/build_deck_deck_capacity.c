#include "../types.h"
#include "build_deck_deck_capacity.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"

int BuildDeck_HasOpenDeckSlot(void)
{
    int index;
    CardEntry *entry = gBuildDeck_pState->lists[1].entries;
    for (index = 0; index < DECK_SIZE; index++, entry++) {
        if (entry->flags == 0) {
            return 1;
        }
    }
    return 0;
}
