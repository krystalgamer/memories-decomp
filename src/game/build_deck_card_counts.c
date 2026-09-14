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
    /* The state is cast at each use: through a BuildDeckTransitionState *
       local the function grows by an instruction. */
    u32 raw = ((BuildDeckTransitionState *)base)->chest_card_quantities[index];
    s32 count = raw & 255;

    if (count == 0) {
        CardListSortItem *entry =
            (CardListSortItem *)((BuildDeckTransitionState *)base)->lists[0].entries;

        ((BuildDeckTransitionState *)base)->chest_total++;
        ((BuildDeckTransitionState *)base)->chest_card_quantities[index]++;
        do {
            s32 id = entry->card_id;

            entry++;
            if (id == index)
                break;
        } while (1);
        entry--;
        /* Keep the post-search adjustment separate from the flag store. */
        ((volatile CardListSortItem *)entry)->field_0D = 1;
        func_80032C48(&((BuildDeckTransitionState *)base)->lists[0]);
    } else if (count != CARD_CHEST_QUANTITY_MAX) {
        s32 next = raw + 1;

        ((BuildDeckTransitionState *)base)->chest_card_quantities[index] = next;
        ((BuildDeckTransitionState *)base)->chest_total++;
    }
}

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
