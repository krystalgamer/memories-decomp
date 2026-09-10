/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers and 2 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_card_object_queries.c.
 */
#include "../types.h"
#include "../game/duel_side_state.h"

#include "../game/duel_card.h"

void func_8002C9B4(u32 *output, int selector)
{
    register DuelCardRecord *entry __asm__("$16");
    register int base __asm__("$17");
    DuelCardRecord *other;
    int index;

    if (selector < 0) {
        __asm__ volatile(
            "lui $2,%%hi(D_801A7B64)\n\t"
            "addiu %0,$2,%%lo(D_801A7B64)"
            : "=r"(entry));
        other = entry + DUEL_CARD_SIDE_RECORD_COUNT;
        for (
            index = 0;
            index < DUEL_FIELD_SIDE_ZONE_COUNT;
            index++, entry++, other++
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
    __asm__ volatile(
        "sll $2,%1,3\n\t"
        "subu $2,$2,%1\n\t"
        "sll $2,$2,2\n\t"
        "lui $3,%%hi(D_801A7AD8)\n\t"
        "addiu $3,$3,%%lo(D_801A7AD8)\n\t"
        "addu %0,$2,$3"
        : "=r"(entry)
        : "r"(base)
        : "$2", "$3");
    if (selector >= 21) {
        for (index = 0; index < DUEL_FIELD_ROW_SIZE; index++, entry++) {
            if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                (u16)Duel_CalcCardStats(entry) >= selector)
                *output++ = (u32)entry->object;
        }
    } else {
        for (index = 0; index < DUEL_FIELD_ROW_SIZE; index++, entry++) {
            if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                (selector < 0 || *((u8 *)entry->object + 0x68) == selector))
                *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}
