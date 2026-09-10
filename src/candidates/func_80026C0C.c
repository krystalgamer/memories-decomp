/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_field_equip_search.c.
 */
#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/card_constants.h"
#include "../game/duel_card.h"
#include "../game/duel_field_equip_search.h"
#include "../game/duel_card_checks.h"

s32 func_80026C0C(s32 arg0)
{
    /* off and p share a0 on purpose: retail scales base by the card-record
     * size into a0 and then adds the D_801A7AD8 symbol in place. The two live
     * ranges touch for exactly zero instructions, because
     * "p = D_801A7AD8 + off" reads off and writes p in a single instruction.
     * Do not insert any statement between the off and p assignments below;
     * that would make the shared pin invalid and the resulting mismatch is
     * not obvious from the source. */
    register s32 base asm("v1");
    register s32 off asm("a0");
    register u8 *p asm("a0");
    s32 i;

    base = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT + arg0;
    off = base * DUEL_CARD_RECORD_SIZE;
    p = (u8 *)D_801A7AD8 + off;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        if ((*(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED) == 0) {
            return base + i;
        }
        p += DUEL_CARD_RECORD_SIZE;
    }
    return -1;
}

