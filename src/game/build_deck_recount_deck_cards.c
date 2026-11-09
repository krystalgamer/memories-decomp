#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"

void BuildDeck_RecountDeckCards(BuildDeckTransitionState *state)
{
    s32 index;

    state->deck_card_quantities[0] = 0;
    {
        CardEntry *record;
        s32 record_index;

        for (index = CARD_ID_FIRST; index < CARD_ID_END; index++) {
            state->deck_card_quantities[index] = 0;
            record = state->lists[1].entries;
            for (record_index = 0; record_index < DECK_SIZE; record_index++) {
                /* Deck rows are compared as signed IDs (empty rows hold -1). */
                if (record->flags != 0 && (s16)record->id == index)
                    state->deck_card_quantities[index]++;
                record++;
            }
        }
    }
    {
        CardEntry *record;
        s32 leading;

        record = state->lists[1].entries;
        leading = 0;
        for (index = 0; index < DECK_SIZE; index++) {
            if (record->flags == 0)
                break;
            leading++;
            record++;
        }
        state->deck_total = leading;
    }
}
