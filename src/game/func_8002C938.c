#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/duel_card.h"

void func_8002C938(u32 *output, int alternate)
{
    int base =
        D_8009B1D5 ? DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    int scaled;
    DuelCardRecord *entry;

    scaled = base << 3;
    if (alternate) {
        base += DUEL_FIELD_ROW_SIZE;
        scaled = base << 3;
    }
    scaled -= base;
    scaled <<= 2;
    entry = (DuelCardRecord *)((u8 *)D_801A7AD8 + scaled);
    base = 0;
    for (; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}
