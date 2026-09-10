#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "card_list_sort.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"

/* The Build Deck screen's card counts: taking one copy out. It lowers the
   card's count at +0x5D97 of the screen record and the total at +0x5A9C,
   and when the count reaches zero it updates the card's entry in the chest
   list at +4 and re-sorts that list through func_80032C48.

   Compiles to an identical object at gcc_2_8_1_g0_split. Bounded below by
   func_80031EE4 and above by func_8003201C, both now candidates under
   src/candidates/. */

void func_80031F7C(u8 *state, s32 id)
{
    s32 count = (state + id)[0x5D97];

    if (count != 0) {
        count--;
        if (count == 0) {
            u8 *record = state + 4;

            while (1) {
                if (*(s16 *)(record + 4) == id) {
                    break;
                }
                record += 0x10;
            }

            record[0xD] = 0;
            if ((state + id)[0x5D97] != 0) {
                record[0xD] = 0x80;
            }
            func_80032C48((CardList *)(state + 4));
        }
        (state + id)[0x5D97] = count;
        *(s32 *)(state + 0x5A9C) -= 1;
    }
}

