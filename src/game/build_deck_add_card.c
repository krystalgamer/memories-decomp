#include "../types.h"
#include "../game/build_deck_card_counts.h"
#include "../game/card_constants.h"
#include "../game/card_list_sort.h"
#include "../game/display_object_layout.h"
#include "../game/duel_card.h"
#include "../game/text_box_lifecycle.h"
#include "../game/text_box_runtime.h"
#include "../game/text_staging.h"

/* The Build Deck screen's card counts: the count box refresh, returning a
   copy to the chest, taking one out, recounting the deck, and adding a card
   to the deck list. All five work on the same screen record - the per-card
   counts at +0x5D97, the two totals at +0x5A9C and +0x5AA0, the chest list
   at +4 and the deck list at the second `lists` element - and each re-sorts
   through BuildDeck_SortCardList when it changes what a list shows.

   The four former sources were recorded at gcc_2_8_1_g0_split, gcc_2_8_1_g0
   and gcc_2_8_1_g8. Every member compiles to an identical object at
   gcc_2_8_1_g0_split. Bounded below by the card-list text boxes, which need
   gcc_2_8_1_g0, and above by CardList_LoadSharedGraphicsStage at gcc_2_8_1_cc_g8_as_g0_split. */

void BuildDeck_AddCard(s32 base, s32 card_id)
{
    s32 *record;
    CardEntry *entry;
    s32 index = 0;
    {
        s32 *base = gDuel_adwCardStats;
        s32 record_index = card_id - 1;

        record = &base[record_index];
    }
    entry = ((BuildDeckTransitionState *)base)->lists[1].entries;
    for (; index < DECK_SIZE; index++, entry++) {
        if (entry->flags == 0) {
            entry->flags = 1;
            entry->id = card_id;
            entry->type = (*record >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            entry->attack = (*record & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            entry->defense =
                ((*record >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                CARD_STAT_SCALE;
            BuildDeck_SortCardList(&((BuildDeckTransitionState *)base)->lists[1]);
            BuildDeck_RecountDeckCards((BuildDeckTransitionState *)base);
            return;
        }
    }
}
