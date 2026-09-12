#include "../types.h"
#include "duel_side_state.h"
#include "duel_card.h"

void func_8002C9B4(u32 *output, s32 selector)
{
    DuelCardRecord *entry;
    s32 base;
    DuelCardRecord *other;

    if (selector < 0) {
        entry = D_801A7B64;
        other = entry + DUEL_CARD_SIDE_RECORD_COUNT;
        for (
            base = 0;
            base < DUEL_FIELD_SIDE_ZONE_COUNT;
            base++, entry++, other++
        ) {
            if (entry->flags & DUEL_CARD_FLAG_OCCUPIED)
                *output++ = (u32)entry->object;
            if (other->flags & DUEL_CARD_FLAG_OCCUPIED)
                *output++ = (u32)other->object;
        }
        *output = 0;
        return;
    }

    base = D_8009B1D5 ?
        DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    entry = &D_801A7AD8[base];
    if (selector >= 21) {
        for (base = 0; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
            if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                (u16)Duel_CalcCardStats(entry) >= selector)
                *output++ = (u32)entry->object;
        }
    } else {
        for (base = 0; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
            if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                (selector < 0 || *((u8 *)entry->object + 0x68) == selector))
                *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}
