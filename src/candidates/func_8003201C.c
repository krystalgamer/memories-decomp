/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 5 variables to hard registers, so it is kept here as a candidate
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

void func_8003201C(u8 *state)
{
    state[0x5AC4] = 0;

    {
        register s32 id asm("a2") = CARD_ID_FIRST;
        u8 *count = state + id;

        for (; id < CARD_ID_END; id++, count++) {
            s32 record_index;
            u8 *record;
            register u8 *output asm("a3");

            count[0x5AC4] = 0;
            record_index = 0;
            output = count;
            record = state + 0x2D54;

            for (; record_index < DECK_SIZE; record_index++) {
                if (record[9] != 0 && *(s16 *)record == id) {
                    output[0x5AC4]++;
                }
                record += 0x10;
            }
        }
    }

    {
        register u8 *record asm("v1") = state + 0x2D50;
        register s32 leading asm("a1") = 0;
        register s32 record_index asm("a2");

        for (record_index = 0; record_index < DECK_SIZE; record_index++) {
            if (record[0xD] == 0) {
                break;
            }
            leading++;
            record += 0x10;
        }

        *(s32 *)(state + 0x5AA0) = leading;
    }
}

