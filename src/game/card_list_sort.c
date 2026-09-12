#include "../types.h"
#include "../psyq/qsort.h"
#include "card_list_text_boxes.h"
#include "card_list_sort.h"
#include "rand_get_interval.h"
#include "card_constants.h"
#include "duel_card.h"
#include "build_deck_transition_state.h"

/* The two comparators the sort below hands to qsort, then the sort itself.
   The comparators were recorded at gcc_2_8_1_g0_split and compile to an
   identical object at the sort's gcc_2_8_1_g8_split. */

s32 BuildDeck_CompareCard(
    CardListSortItem *arg0,
    CardListSortItem *arg1
)
{
    u32 a = arg0->key;
    u32 b = arg1->key;

    if (a == b) {
        if (
            gCard_asNameSortKey[arg0->card_id - 1] <
            gCard_asNameSortKey[arg1->card_id - 1]
        )
            return -1;
        return 1;
    }
    if (a < b)
        return 1;
    return -1;
}

s32 func_80032BD4(
    CardListSortItem *arg0,
    CardListSortItem *arg1
)
{
    u32 a = arg0->key;
    u32 b = arg1->key;

    if (a == b) {
        if (
            gCard_asNameSortKey[arg0->card_id - 1] <
            gCard_asNameSortKey[arg1->card_id - 1]
        )
            return -1;
        return 1;
    }
    if (b < a)
        return 1;
    return -1;
}

/* Card-list sort. Builds a 32-bit sort key into each sixteen-byte row of the
   list at p and hands the block to qsort with one of two comparators, chosen
   by the list's sort_mode.

   Levers that mattered here:
   - The rows are walked with a pointer, not indexed. Indexing p[i] lets
     combine_givs merge the key access at +0 with the id/flag accesses at +4
     into a single giv and loses eight increments; walking the pointer keeps
     retail's second giv at +4.
   - The stat reads are written inline at every use rather than through a
     local. A local for gDuel_adwCardStats[id - 1] keeps the value in its own
     register and shifts the whole loop's allocation by one.
   - The key expressions are inline for the same reason: with atk and def as
     locals gcc schedules both multiplies before either shift, where retail
     finishes the attack half before starting the defence half.
   - Halved stats are spelled value * (CARD_STAT_SCALE / 2) and the comparison
     multiplies back up. Written the other way round, as
     (value * CARD_STAT_SCALE) >> 1, gcc folds the shift into the constant and
     emits sll 16 where retail has sll 17.
   - The first loop increments the index before the row pointer and every
     other loop the row pointer first, which is the order retail schedules the
     two induction variables in each. */

void func_80032C48(CardList *list)
{
    CardListSortItem *row;
    s32 i;
    s32 n;

    n = list->sort_row_count;
    row = (CardListSortItem *)list;
    switch (list->sort_mode) {
    case 1:
        if (list->kind != 0) {
            for (i = 0; i < n; i++, row++) {
                row->key = 0xFFFF;
                if (row->field_0D != 0) {
                    row->key = row->card_id;
                }
            }
        } else {
            for (i = 0; i < n; row++, i++) {
                row->key = row->card_id;
            }
        }
        qsort(list, n, 0x10, func_80032BD4);
        break;
    case 2:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->field_0D != 0) {
                row->key = 0;
            }
        }
        qsort(list, n, 0x10, func_80032BD4);
        break;
    case 3:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->field_0D != 0) {
                if ((gDuel_adwCardStats[row->card_id - 1] & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE >=
                    ((gDuel_adwCardStats[row->card_id - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                        CARD_STAT_SCALE) {
                    row->key =
                        (((gDuel_adwCardStats[row->card_id - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 17) |
                        ((((gDuel_adwCardStats[row->card_id - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                } else {
                    row->key =
                        ((((gDuel_adwCardStats[row->card_id - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 17) |
                        (((gDuel_adwCardStats[row->card_id - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                }
            }
        }
        qsort(list, n, 0x10, BuildDeck_CompareCard);
        break;
    case 4:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->field_0D != 0) {
                row->key =
                    (((gDuel_adwCardStats[row->card_id - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 17) |
                    ((((gDuel_adwCardStats[row->card_id - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(list, n, 0x10, BuildDeck_CompareCard);
        break;
    case 5:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->field_0D != 0) {
                row->key =
                    ((((gDuel_adwCardStats[row->card_id - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 17) |
                    (((gDuel_adwCardStats[row->card_id - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(list, n, 0x10, BuildDeck_CompareCard);
        break;
    case 6:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->field_0D != 0) {
                row->key = (gDuel_adwCardStats[row->card_id - 1] >>
                            CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            }
        }
        qsort(list, n, 0x10, func_80032BD4);
        break;
    case 8:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->field_0D != 0) {
                row->key = 0x100;
                if (D_8009B2FC->card_sort_rank[row->card_id] != 0) {
                    row->key = D_8009B2FC->card_sort_rank[row->card_id];
                }
            }
        }
        qsort(list, n, 0x10, func_80032BD4);
        break;
    case 9:
        n = list->row_count;
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->field_0D != 0) {
                row->key = Rand_GetInterval(0x1000);
            }
        }
        qsort(list, n, 0x10, func_80032BD4);
        break;
    }
    func_80031E04(list, 8);
}
