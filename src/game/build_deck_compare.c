#include "../types.h"
#include "card_list_sort.h"

extern s16 gCard_asNameSortKey[];

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
