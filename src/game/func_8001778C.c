#include "../types.h"
#include "func_8001778C.h"
#include "duel_card.h"

void func_8001778C(void)
{
    u8 *entry = (u8 *)D_801A7AD8;
    int i = 0;
    u8 *field_16 = entry + 0x16;

    do {
        *(int *)entry = 0;
        *(int *)(field_16 - 0x12) = 0;
        *(u16 *)field_16 = 0;
        field_16 += sizeof(DuelCardRecord);
        i++;
        entry += sizeof(DuelCardRecord);
    } while (i < DUEL_CARD_RECORD_COUNT);
}
