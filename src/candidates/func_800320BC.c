/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/build_deck_card_counts.c.
 */
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
   at +4 and the deck list at +0x2D50 - and each re-sorts through
   func_80032C48 when it changes what a list shows.

   The four former sources were recorded at gcc_2_8_1_g0_split, gcc_2_8_1_g0
   and gcc_2_8_1_g8. Every member compiles to an identical object at
   gcc_2_8_1_g0_split. Bounded below by the card-list text boxes, which need
   gcc_2_8_1_g0, and above by func_80032184 at gcc_2_8_1_cc_g8_as_g0_split. */

void BuildDeck_AddCard(s32 arg0, s32 arg1)
{
    s32 *record;
    /* Keep one induction pointer instead of folding accesses around entry-2. */
    register s16 *entry asm("$6");
    register s32 index asm("$4") = 0;
    {
        s32 *base = gDuel_adwCardStats;
        s32 record_index = arg1 - 1;

        record = &base[record_index];
    }
    entry = (s16 *)(arg0 + 0x2D58);
    do {
        if (((u8 *)entry)[5] == 0) {
            ((u8 *)entry)[5] = 1;
            entry[-2] = (s16)arg1;
            ((u8 *)entry)[2] =
                (u8)((*record >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK);
            entry[-1] =
                (s16)((*record & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE);
            entry[0] =
                (s16)(((*record >> CARD_STAT_DEFENSE_SHIFT) &
                       CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE);
            func_80032C48((void *)(arg0 + 0x2D50));
            func_8003201C(arg0);
            return;
        }
        index++;
        entry += 8;
    } while (index < DECK_SIZE);
}
