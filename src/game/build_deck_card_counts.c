#include "../types.h"
#include "build_deck_card_counts.h"
#include "build_deck_transition_state.h"
#include "card_constants.h"
#include "card_list_sort.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"

/* The Build Deck screen's three contiguous count helpers. They refresh the
   count box, return a card to the chest, and remove a card from it. The unit
   is the complete gcc_2_8_1_g0_split run between the card-list text boxes and
   func_8003201C. */

void func_80031E5C(u8 *arg0) {
    u8 *p;
    D_801D5608[0].build_deck.chest =
        ((BuildDeckTransitionState *)arg0)->chest_total;
    D_801D5608[0].build_deck.deck =
        ((BuildDeckTransitionState *)arg0)->deck_total;
    p = TextBox_CreateFlagged(3, 0xE, 0x16, 0x17, 0x280, 0x10, 0x100);
    func_80039A14((struct DuelEffectChannel *)p);
    *(u16 *)(*(u8 **)(p + 0x28) + 8) &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void func_80031EE4(u8 *base, s32 index)
{
    u8 *counts = base + index;
    u32 raw = counts[0x5D97];
    s32 count = raw & 255;

    if (count == 0) {
        CardListSortItem *entry = (CardListSortItem *)(base + 4);

        (*(s32 *)(base + 0x5A9C))++;
        counts[0x5D97]++;
        do {
            s32 id = entry->card_id;

            entry++;
            if (id == index)
                break;
        } while (1);
        entry--;
        /* Keep the post-search adjustment separate from the flag store. */
        ((volatile CardListSortItem *)entry)->field_0D = 1;
        func_80032C48((CardList *)(base + 4));
    } else if (count != CARD_CHEST_QUANTITY_MAX) {
        s32 next = raw + 1;

        counts[0x5D97] = next;
        (*(s32 *)(base + 0x5A9C))++;
    }
}

void func_80031F7C(u8 *state, s32 id)
{
    s32 count = ((BuildDeckTransitionState *)state)->chest_card_quantities[id];

    if (count != 0) {
        count--;
        if (count == 0) {
            CardEntry *record =
                ((BuildDeckTransitionState *)state)->lists[0].entries;

            while (1) {
                if ((s16)record->id == id) {
                    break;
                }
                record++;
            }

            record->flags = 0;
            if (((BuildDeckTransitionState *)state)->chest_card_quantities[id] != 0) {
                record->flags = 0x80;
            }
            func_80032C48(&((BuildDeckTransitionState *)state)->lists[0]);
        }
        ((BuildDeckTransitionState *)state)->chest_card_quantities[id] = count;
        ((BuildDeckTransitionState *)state)->chest_total -= 1;
    }
}
