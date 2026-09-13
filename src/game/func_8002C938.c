#include "../types.h"
#include "duel_card.h"
#include "duel_side_state.h"

void func_8002C938(u32 *output, s32 alternate)
{
    s32 base =
        D_8009B1D5 ? DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    s32 scaled;
    DuelCardRecord *entry;

    if (alternate != 0) {
        base += DUEL_FIELD_ROW_SIZE;
    }
    scaled = base << 3;
    scaled -= base;
    scaled <<= 2;
    entry = (DuelCardRecord *)((u8 *)D_801A7AD8 + scaled);
    base = 0;
    for (; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
        if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}
