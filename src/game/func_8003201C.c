#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"

void func_8003201C(u8 *state)
{
    s32 index;

    ((BuildDeckTransitionState *)state)->deck_card_quantities[0] = 0;
    /* The count loop keeps its byte cursors: `count` and `output` walk the
       deck quantity table from `state + index`, and `record` sits on each
       deck row's id (state + 0x2D54, 16 bytes a row) with the flags byte 9
       past it. Written as deck_card_quantities[] and CardEntry members the
       loop's addressing changes (4 and 7 differences). */
    {
        u8 *count;

        index = CARD_ID_FIRST;
        count = state + index;
        for (; index < CARD_ID_END; index++, count++) {
            s32 record_index;
            u8 *record;
            u8 *output;

            count[0x5AC4] = 0;
            record_index = 0;
            output = count;
            record = state + 0x2D54;
            for (; record_index < DECK_SIZE; record_index++) {
                if (record[9] != 0 && *(s16 *)record == index)
                    output[0x5AC4]++;
                record += 0x10;
            }
        }
    }
    {
        CardEntry *record;
        s32 leading;

        record = ((BuildDeckTransitionState *)state)->lists[1].entries;
        leading = 0;
        for (index = 0; index < DECK_SIZE; index++) {
            if (record->flags == 0)
                break;
            leading++;
            record++;
        }
        ((BuildDeckTransitionState *)state)->deck_total = leading;
    }
}
