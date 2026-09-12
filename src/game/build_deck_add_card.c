#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "card_list_sort.h"
#include "duel_card.h"

void BuildDeck_AddCard(u8 *state, s32 card_id)
{
    s32 *record;
    s16 *entry;
    s32 index = 0;

    {
        s32 *base = gDuel_adwCardStats;
        s32 record_index = card_id - 1;

        record = &base[record_index];
    }
    entry = (s16 *)(state + 0x2D58);
    do {
        if (((u8 *)entry)[5] == 0) {
            ((u8 *)entry)[5] = 1;
            entry[-2] = (s16)card_id;
            ((u8 *)entry)[2] =
                (u8)((*record >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK);
            entry[-1] =
                (s16)((*record & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE);
            entry[0] =
                (s16)(((*record >> CARD_STAT_DEFENSE_SHIFT) &
                       CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE);
            func_80032C48((CardList *)(state + 0x2D50));
            func_8003201C(state);
            return;
        }
        index++;
        entry += 8;
    } while (index < DECK_SIZE);
}
