#include "../types.h"
#include "duel_card.h"

void Duel_ResetCardRecords(void)
{
    DuelCardRecord *entry = D_801A7AD8;
    int i = 0;
    DuelCardFlagsCursor *flags = (DuelCardFlagsCursor *)&entry->flags;

    do {
        *(s32 *)&entry->object = 0;
        /* Keep the flags-relative cursor while deriving the data distance. */
        *(s32 *)(&flags->flags -
            ((u32)&((DuelCardRecord *)0)->flags -
             (u32)&((DuelCardRecord *)0)->data) / sizeof(flags->flags)) = 0;
        flags->flags = 0;
        flags++;
        i++;
        entry++;
    } while (i < DUEL_CARD_RECORD_COUNT);
}
