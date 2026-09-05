#include "../types.h"
#include "duel_card.h"

extern u8 D_8009B1D5;

s32 func_80026C0C(s32 arg0)
{
    /* off and p share a0 on purpose: retail scales base * 28 into a0 and then
     * adds the D_801A7AD8 symbol in place. The two live ranges touch for
     * exactly zero instructions, because "p = D_801A7AD8 + off" reads off and
     * writes p in a single instruction. Do not insert any statement between
     * the off and p assignments below; that would make the shared pin invalid
     * and the resulting mismatch is not obvious from the source. */
    register s32 base asm("v1");
    register s32 off asm("a0");
    register u8 *p asm("a0");
    s32 i;

    base = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT + arg0;
    off = base * 28;
    p = (u8 *)D_801A7AD8 + off;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        if ((*(u16 *)(p + 0x16) & 0x8000) == 0) {
            return base + i;
        }
        p += 28;
    }
    return -1;
}
