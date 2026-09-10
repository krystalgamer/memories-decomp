/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
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

void func_80031EE4(unsigned char *base, int index)
{
    register unsigned char *p __asm__("$6") = base + index;
    register unsigned int raw __asm__("$7") = p[0x5D97];
    int c = raw & 255;

    if (c == 0) {
        unsigned char *e = base + 4;

        (*(int *)(base + 0x5A9C))++;
        p[0x5D97]++;
        do {
            int v = *(short *)(e + 4);

            e += 16;
            if (v == index)
                break;
        } while (1);
        e -= 16;
        __asm__ volatile("" : "+r"(e));
        e[0xD] = 1;
        func_80032C48((CardList *)(base + 4));
    } else if (c != CARD_CHEST_QUANTITY_MAX) {
        register int next __asm__("$2") = raw + 1;

        p[0x5D97] = next;
        (*(int *)(base + 0x5A9C))++;
    }
}

