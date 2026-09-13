/* The cursor handler requires volatile absolute input loads under -G8. */
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/qsort.h"
#include "card_type_icon_table.h"
#include "card_list_text_boxes.h"
#include "card_list_sort.h"
#include "rand_get_interval.h"
#include "card_constants.h"
#include "duel_card.h"
#include "build_deck_transition_state.h"
#include "input.h"
#include "sound.h"

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

s32 func_800330BC(CardList *list)
{
    s32 row;
    s32 sel;
    s32 icon;
    s32 slot;
    u8 *entry;

    row = list->first | list->cursor;
    if (row != 0) {
        row = (list->first + list->cursor + 1) * 152 /
              list->sort_row_count;
    }
    *(s16 *)(list->scroll_box + 0x32) = row + 0x29;

top:
    if (list->first != list->first_target) {
        if (list->first_target < list->first) {
            *(u16 *)&list->first = *(u16 *)&list->first - 1;
        } else {
            *(u16 *)&list->first = *(u16 *)&list->first + 1;
        }
        func_80031E04(list, 8);
        return 1;
    }

    if ((gInput_wPad1Held & PAD_BUTTON_L1_R1_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_wPad1Held & PAD_BUTTON_R1) != 0) {
            if (row == list->row_count - 8 &&
                list->cursor != 7) {
                sel = 7;
            }
            row += 8;
            if (list->row_count - 8 < row) {
                row = list->row_count - 8;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= 8;
        }
        if (row < 0) {
            row = 0;
        }
        list->first_target = row;
        if (list->first != row) {
            SD_SEPlayFull(6);
            goto top;
        }
    finish:
        if (sel >= 0) {
            list->cursor = sel;
            *(s16 *)(list->cursor_box + 0x32) = sel * 22 + 0x2A;
            SD_SEPlayFull(6);
        }
        return 1;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_TRIGGER_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_wPad1Repeat & PAD_BUTTON_R2) != 0) {
            if (row == list->row_count - 8 &&
                list->cursor != 7) {
                sel = 7;
            }
            row += 0x32;
            if (list->row_count - 8 < row) {
                row = list->row_count - 8;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= 0x32;
            if (row < 0) {
                row = 0;
            }
        }
        if (list->first != row) {
            SD_SEPlayFull(6);
            list->first_target = row;
            list->first = row;
            func_80031E04(list, 8);
            return 1;
        }
        goto finish;
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_VERTICAL_MASK) != 0) {
        row = list->first;
        if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
            list->cursor += 1;
            if (list->cursor >= 8) {
                row += 1;
                list->cursor = 7;
            }
        } else {
            list->cursor -= 1;
            if (list->cursor < 0) {
                row -= 1;
                list->cursor = 0;
            }
        }
        *(s16 *)(list->cursor_box + 0x32) =
            list->cursor * 22 + 0x2A;
        if (list->row_count - 8 < row) {
            return 1;
        }
        if (row < 0) {
            return 1;
        }
        SD_SEPlayFull(6);
        list->first_target = row;
        if (list->first != row) {
            goto top;
        }
        return 1;
    }

    if ((gInput_wPad1Repeat & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) !=
        0) {
        if ((gInput_wPad1Repeat & PAD_BUTTON_START) != 0) {
            list->sort_choice += 1;
            if (list->sort_choice >= 7) {
                list->sort_choice = 0;
            }
        } else {
            list->sort_choice -= 1;
            if (list->sort_choice < 0) {
                list->sort_choice = 6;
            }
        }
        SD_SEPlayFull(0x2F);
        icon = list->kind;
        slot = list->sort_choice;
        icon = icon << 4;
        slot = slot << 1;
        entry = D_80090DD8 + slot;
        icon = icon + (s32)entry;
        list->sort_mode = *(u8 *)(icon + 1) & 0xF;
        func_80032C48(list);
        return 1;
    }

    return 0;
}
