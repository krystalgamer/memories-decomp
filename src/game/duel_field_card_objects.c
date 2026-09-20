#include "../types.h"
#include "duel_card.h"
#include "duel_side_state.h"
#include "display_object.h"

#define DUEL_CARD_RECORD_AT_OFFSET(records, offset) \
    ((DuelCardRecord *)((u8 *)(records) + (offset)))

void Duel_CollectFieldRowCardObjects(u32 *output, s32 back_row)
{
    s32 base =
        D_8009B1D5 ? DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    s32 scaled;
    DuelCardRecord *entry;

    if (back_row != 0) {
        base += DUEL_FIELD_ROW_SIZE;
    }
    scaled = base << 3;
    scaled -= base;
    scaled <<= 2;
    entry = DUEL_CARD_RECORD_AT_OFFSET(D_801A7AD8, scaled);
    base = 0;
    for (; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
        if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}

void Duel_CollectMatchingFieldCardObjects(u32 *output, s32 selector)
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
                (selector < 0 ||
                 ((DisplayObject *)entry->object)->field_68 == selector))
                *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}
