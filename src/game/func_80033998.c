#include "../types.h"
#include "func_80033998.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"

int func_80033998(void)
{
    int index;
    CardEntry *entry = D_8009B2FC->lists[1].entries;
    for (index = 0; index < DECK_SIZE; index++, entry++) {
        if (entry->flags == 0) {
            return 1;
        }
    }
    return 0;
}
