#include "../types.h"
#include "card_constants.h"

/* Card-list sort. Builds a 32-bit sort key into each sixteen-byte row of the
   list at p and hands the block to qsort with one of two comparators, chosen
   by the sort mode at p[0x2D45].

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

typedef struct {
    u32 key;
    s16 index;
    u8 pad_06[7];
    u8 flag_0D;
    u8 pad_0E[2];
} SortItem;

extern s32 gDuel_adwCardStats[];
extern u8 *D_8009B2FC;

extern s32 BuildDeck_CompareCard();
extern s32 func_80032BD4();
extern void func_80031E04(u8 *, s32);
extern s32 Rand_GetInterval(s32);

void func_80032C48(u8 *p)
{
    SortItem *row;
    s32 i;
    s32 n;

    n = *(s16 *)(p + 0x2D42);
    row = (SortItem *)p;
    switch (p[0x2D45]) {
    case 1:
        if (p[0x2D47] != 0) {
            for (i = 0; i < n; i++, row++) {
                row->key = 0xFFFF;
                if (row->flag_0D != 0) {
                    row->key = row->index;
                }
            }
        } else {
            for (i = 0; i < n; row++, i++) {
                row->key = row->index;
            }
        }
        qsort(p, n, 0x10, func_80032BD4);
        break;
    case 2:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flag_0D != 0) {
                row->key = 0;
            }
        }
        qsort(p, n, 0x10, func_80032BD4);
        break;
    case 3:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flag_0D != 0) {
                if ((gDuel_adwCardStats[row->index - 1] & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE >=
                    ((gDuel_adwCardStats[row->index - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                        CARD_STAT_SCALE) {
                    row->key =
                        (((gDuel_adwCardStats[row->index - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 17) |
                        ((((gDuel_adwCardStats[row->index - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                } else {
                    row->key =
                        ((((gDuel_adwCardStats[row->index - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 17) |
                        (((gDuel_adwCardStats[row->index - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                }
            }
        }
        qsort(p, n, 0x10, BuildDeck_CompareCard);
        break;
    case 4:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flag_0D != 0) {
                row->key =
                    (((gDuel_adwCardStats[row->index - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 17) |
                    ((((gDuel_adwCardStats[row->index - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(p, n, 0x10, BuildDeck_CompareCard);
        break;
    case 5:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flag_0D != 0) {
                row->key =
                    ((((gDuel_adwCardStats[row->index - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 17) |
                    (((gDuel_adwCardStats[row->index - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(p, n, 0x10, BuildDeck_CompareCard);
        break;
    case 6:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flag_0D != 0) {
                row->key = (gDuel_adwCardStats[row->index - 1] >>
                            CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            }
        }
        qsort(p, n, 0x10, func_80032BD4);
        break;
    case 8:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flag_0D != 0) {
                row->key = 0x100;
                if (*(D_8009B2FC + row->index + 0x606A) != 0) {
                    row->key = *(D_8009B2FC + row->index + 0x606A);
                }
            }
        }
        qsort(p, n, 0x10, func_80032BD4);
        break;
    case 9:
        n = *(s16 *)(p + 0x2D40);
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flag_0D != 0) {
                row->key = Rand_GetInterval(0x1000);
            }
        }
        qsort(p, n, 0x10, func_80032BD4);
        break;
    }
    func_80031E04(p, 8);
}
