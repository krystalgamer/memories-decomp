/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_card_object_queries.c.
 */
#include "../types.h"
#include "../game/duel_side_state.h"

#include "../game/duel_card.h"

void func_8002C938(u32 *output, int alternate)
{
    register int base __asm__("$6") =
        D_8009B1D5 ? DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    register int scaled __asm__("$3");
    register DuelCardRecord *entry __asm__("$3");

    if (alternate) {
        base += DUEL_FIELD_ROW_SIZE;
    }
    scaled = base << 3;
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

