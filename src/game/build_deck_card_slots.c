#include "../types.h"

/* The Build Deck card slots and counts, five contiguous functions from
   CardList_CreateSlotTextBox (0x80031CD4) through
   BuildDeck_TakeCardFromChest: the per-slot card-list text box and the loop
   that builds every visible slot, then the count-box refresh and the two
   transfers that move one card between the chest and the deck. */

#include "text_box_blocking_build.h"
#include "card_constants.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "card_list_text_boxes.h"
#include "build_deck_card_counts.h"
#include "build_deck_transition_state.h"
#include "card_list_sort.h"
#include "duel_card.h"
#include "text_staging.h"
#include "text_message_staging.h"

/* Builds the text box for one card slot of a trade or deck screen. The entry
   is picked out of the list at the scroll offset plus the slot, and the list
   kind at +0x2D47 selects both the box template and a fixed 0x160 shift. */

#ifndef CARD_LIST_SLOT_BOX_Y
#define CARD_LIST_SLOT_BOX_Y 0x2B
#endif

#ifndef CARD_LIST_SLOT_BOX_HEIGHT
#define CARD_LIST_SLOT_BOX_HEIGHT 0xB0
#endif

#ifndef CARD_LIST_SLOT_FOLLOWING_FLAG
#define CARD_LIST_SLOT_FOLLOWING_FLAG 0x40
#endif

void CardList_CreateSlotTextBox(CardList *list, s32 slot)
{
    DuelEffectChannel *box;
    s32 style;

    gDuel_wSelectedCardID = list->entries[list->first + slot].id;
    style = 0;
    if (list->entries[list->first + slot].flags != 0) {
        style = 6;
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
        /* The later regional builds give a magic or other non-monster card
           its own box style. */
        if (((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
              CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
            style = 7;
        }
#endif
    }
    box = TextBox_Create(list->kind + 1, style, 0x22,
                         CARD_LIST_SLOT_BOX_Y, 0x120,
                         CARD_LIST_SLOT_BOX_HEIGHT);
    box->pen_y = slot * 22;
    box->render_object->flags &=
        ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if ((list->entries[list->first + slot].flags & 0x80) != 0) {
        box->color_index_54 = 4;
    }
    if (list->kind != 0) {
        *(u16 *)&box->rect_x += 0x160;
    }
    if (slot != 0) {
        box->flags_34 |= CARD_LIST_SLOT_FOLLOWING_FLAG;
    }
    TextBox_BuildBlocking(box);
}

void CardList_CreateSlotTextBoxes(CardList *list, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++)
        CardList_CreateSlotTextBox(list, i);
}

/* Regional values: the count box's y (0x17 -> 0x19) and flags (0x100 ->
 * 0x1). The European build (src/game/european/) defines its own. */
#ifndef BUILD_DECK_COUNT_BOX_Y
#define BUILD_DECK_COUNT_BOX_Y 0x17
#endif
#ifndef BUILD_DECK_COUNT_BOX_FLAGS
#define BUILD_DECK_COUNT_BOX_FLAGS 0x100
#endif

/* The Build Deck screen's three contiguous count helpers. They refresh the
   count box, return a card to the chest, and remove a card from it. The unit
   is the complete gcc_2_8_1_g0_split run between the card-list text boxes and
   BuildDeck_RecountDeckCards.

   All three take the screen record as itself. An earlier note here said the state had to be cast at each use because a
   BuildDeckTransitionState * local cost an instruction; that is true of a
   local, and not of the parameter, which is the same incoming register
   either way. */

void BuildDeck_RefreshCountDisplay(BuildDeckTransitionState *record) {
    DuelEffectChannel *p;
    gText_StagedValues[0].build_deck.chest = record->chest_total;
    gText_StagedValues[0].build_deck.deck = record->deck_total;
    p = TextBox_CreateFlagged(3, 0xE, 0x16, BUILD_DECK_COUNT_BOX_Y, 0x280, 0x10, BUILD_DECK_COUNT_BOX_FLAGS);
    TextBox_BuildBlocking(p);
    p->render_object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void BuildDeck_ReturnCardToChest(BuildDeckTransitionState *base, s32 index)
{
    u32 raw = base->chest_card_quantities[index];
    s32 count = raw & 255;

    if (count == 0) {
        CardEntry *entry = base->lists[0].entries;

        base->chest_total++;
        base->chest_card_quantities[index]++;
        do {
            s32 id = (s16)entry->id;

            entry++;
            if (id == index)
                break;
        } while (1);
        entry--;
        /* Keep the post-search adjustment separate from the flag store. */
        ((volatile CardEntry *)entry)->flags = 1;
        BuildDeck_SortCardList(&base->lists[0]);
    } else if (count != CARD_CHEST_QUANTITY_MAX) {
        s32 next = raw + 1;

        base->chest_card_quantities[index] = next;
        base->chest_total++;
    }
}

void BuildDeck_TakeCardFromChest(BuildDeckTransitionState *state, s32 id)
{
    s32 count = state->chest_card_quantities[id];

    if (count != 0) {
        count--;
        if (count == 0) {
            CardEntry *record = state->lists[0].entries;

            while (1) {
                if ((s16)record->id == id) {
                    break;
                }
                record++;
            }

            record->flags = 0;
            if (state->chest_card_quantities[id] != 0) {
                record->flags = 0x80;
            }
            BuildDeck_SortCardList(&state->lists[0]);
        }
        state->chest_card_quantities[id] = count;
        state->chest_total -= 1;
    }
}
