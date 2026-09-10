/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_field_equip_search.c.
 */
#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/card_constants.h"
#include "../game/duel_card.h"
#include "../game/duel_field_equip_search.h"
#include "../game/duel_card_checks.h"

s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 arg1,
                                    s32 arg2)
{
    /* Retail keeps the result count in t0 and the five-entry index in a3. */
    register s32 count asm("$8") = 0;
    register s32 i asm("$7") = 0;
    s32 d = D_8009B1D5;
    s32 *t = gDuel_adwCardStats;
    DuelCardRecord *r =
        &D_801A7AD8[d * DUEL_CARD_SIDE_RECORD_COUNT + arg1];
    s32 k;

    do {
        /* Match the block numbering used by the adjacent
         * Duel_CollectFieldCardsByType. */
        do {
            if (r->flags & DUEL_CARD_FLAG_OCCUPIED) {
                k = (s16)r->card_id;
                k--;
                if (((t[k] >> CARD_STAT_TYPE_SHIFT) &
                     CARD_STAT_TYPE_MASK) < arg2) {
                    *out++ = r;
                    count++;
                }
            }
        } while (0);
        i++;
        r++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;
    return count;
}

