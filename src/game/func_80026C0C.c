#include "../types.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_field_equip_search.h"

s32 func_80026C0C(s32 arg0)
{
    s32 base;
    s32 off;
    u8 *p;
    s32 i;

    do {
        base = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT + arg0;
        off = base << 3;
        off -= base;
        off <<= 2;
        p = (u8 *)D_801A7AD8 + off;
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            off = *(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED;
            if (off == 0) {
                return D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT + arg0 + i;
            }
            p += DUEL_CARD_RECORD_SIZE;
        }
    } while (0);
    return -1;
}
