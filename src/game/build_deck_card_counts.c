#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "card_list_sort.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"

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

void func_80031E5C(u8 *arg0) {
    u8 *p;
    D_801D5608[0].build_deck.chest = *(u32 *)(arg0 + 0x5A9C);
    D_801D5608[0].build_deck.deck = *(u32 *)(arg0 + 0x5AA0);
    p = TextBox_CreateFlagged(3, 0xE, 0x16, 0x17, 0x280, 0x10, 0x100);
    func_80039A14(p);
    *(u16 *)(*(u8 **)(p + 0x28) + 8) &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

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
