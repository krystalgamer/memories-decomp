#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"

#define BUILD_DECK_RECOUNT_QUANTITY_OFFSET \
    ((u32)&((BuildDeckTransitionState *)0)->deck_card_quantities)
#define BUILD_DECK_RECOUNT_ROW_ID_OFFSET \
    ((u32)&((BuildDeckTransitionState *)0)->lists[1].entries[0].id)
#define CARD_ENTRY_ID_TO_FLAGS_OFFSET \
    ((u32)&((CardEntry *)0)->flags - (u32)&((CardEntry *)0)->id)

void func_8003201C(BuildDeckTransitionState *state)
{
    s32 index;

    state->deck_card_quantities[0] = 0;
    /* The count loop keeps its byte cursors: typed member accesses change its
       addressing, but the deck-row base, flags distance and stride can still
       be derived from the shared layouts. */
    {
        u8 *count;

        index = CARD_ID_FIRST;
        count = (u8 *)state + index;
        for (; index < CARD_ID_END; index++, count++) {
            s32 record_index;
            u8 *record;
            u8 *output;

            count[BUILD_DECK_RECOUNT_QUANTITY_OFFSET] = 0;
            record_index = 0;
            output = count;
            record = (u8 *)state + BUILD_DECK_RECOUNT_ROW_ID_OFFSET;
            for (; record_index < DECK_SIZE; record_index++) {
                if (record[CARD_ENTRY_ID_TO_FLAGS_OFFSET] != 0 &&
                    *(s16 *)record == index)
                    output[BUILD_DECK_RECOUNT_QUANTITY_OFFSET]++;
                record += sizeof(CardEntry);
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
